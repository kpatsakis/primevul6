md_process_inlines(MD_CTX* ctx, const MD_LINE* lines, int n_lines)
{
    MD_TEXTTYPE text_type;
    const MD_LINE* line = lines;
    MD_MARK* prev_mark = NULL;
    MD_MARK* mark;
    OFF off = lines[0].beg;
    OFF end = lines[n_lines-1].end;
    int enforce_hardbreak = 0;
    int ret = 0;

    /* Find first resolved mark. Note there is always at least one resolved
     * mark,  the dummy last one after the end of the latest line we actually
     * never really reach. This saves us of a lot of special checks and cases
     * in this function. */
    mark = ctx->marks;
    while(!(mark->flags & MD_MARK_RESOLVED))
        mark++;

    text_type = MD_TEXT_NORMAL;

    while(1) {
        /* Process the text up to the next mark or end-of-line. */
        OFF tmp = (line->end < mark->beg ? line->end : mark->beg);
        if(tmp > off) {
            MD_TEXT(text_type, STR(off), tmp - off);
            off = tmp;
        }

        /* If reached the mark, process it and move to next one. */
        if(off >= mark->beg) {
            switch(mark->ch) {
                case '\\':      /* Backslash escape. */
                    if(ISNEWLINE(mark->beg+1))
                        enforce_hardbreak = 1;
                    else
                        MD_TEXT(text_type, STR(mark->beg+1), 1);
                    break;

                case ' ':       /* Non-trivial space. */
                    MD_TEXT(text_type, _T(" "), 1);
                    break;

                case '`':       /* Code span. */
                    if(mark->flags & MD_MARK_OPENER) {
                        MD_ENTER_SPAN(MD_SPAN_CODE, NULL);
                        text_type = MD_TEXT_CODE;
                    } else {
                        MD_LEAVE_SPAN(MD_SPAN_CODE, NULL);
                        text_type = MD_TEXT_NORMAL;
                    }
                    break;

                case '_':       /* Underline (or emphasis if we fall through). */
                    if(ctx->parser.flags & MD_FLAG_UNDERLINE) {
                        if(mark->flags & MD_MARK_OPENER) {
                            while(off < mark->end) {
                                MD_ENTER_SPAN(MD_SPAN_U, NULL);
                                off++;
                            }
                        } else {
                            while(off < mark->end) {
                                MD_LEAVE_SPAN(MD_SPAN_U, NULL);
                                off++;
                            }
                        }
                        break;
                    }
                    MD_FALLTHROUGH();

                case '*':       /* Emphasis, strong emphasis. */
                    if(mark->flags & MD_MARK_OPENER) {
                        if((mark->end - off) % 2) {
                            MD_ENTER_SPAN(MD_SPAN_EM, NULL);
                            off++;
                        }
                        while(off + 1 < mark->end) {
                            MD_ENTER_SPAN(MD_SPAN_STRONG, NULL);
                            off += 2;
                        }
                    } else {
                        while(off + 1 < mark->end) {
                            MD_LEAVE_SPAN(MD_SPAN_STRONG, NULL);
                            off += 2;
                        }
                        if((mark->end - off) % 2) {
                            MD_LEAVE_SPAN(MD_SPAN_EM, NULL);
                            off++;
                        }
                    }
                    break;

                case '~':
                    if(mark->flags & MD_MARK_OPENER)
                        MD_ENTER_SPAN(MD_SPAN_DEL, NULL);
                    else
                        MD_LEAVE_SPAN(MD_SPAN_DEL, NULL);
                    break;

                case '$':
                    if(mark->flags & MD_MARK_OPENER) {
                        MD_ENTER_SPAN((mark->end - off) % 2 ? MD_SPAN_LATEXMATH : MD_SPAN_LATEXMATH_DISPLAY, NULL);
                        text_type = MD_TEXT_LATEXMATH;
                    } else {
                        MD_LEAVE_SPAN((mark->end - off) % 2 ? MD_SPAN_LATEXMATH : MD_SPAN_LATEXMATH_DISPLAY, NULL);
                        text_type = MD_TEXT_NORMAL;
                    }
                    break;

                case '[':       /* Link, wiki link, image. */
                case '!':
                case ']':
                {
                    const MD_MARK* opener = (mark->ch != ']' ? mark : &ctx->marks[mark->prev]);
                    const MD_MARK* closer = &ctx->marks[opener->next];
                    const MD_MARK* dest_mark;
                    const MD_MARK* title_mark;

                    if ((opener->ch == '[' && closer->ch == ']') &&
                        opener->end - opener->beg >= 2 &&
                        closer->end - closer->beg >= 2)
                    {
                        int has_label = (opener->end - opener->beg > 2);
                        SZ target_sz;

                        if(has_label)
                            target_sz = opener->end - (opener->beg+2);
                        else
                            target_sz = closer->beg - opener->end;

                        MD_CHECK(md_enter_leave_span_wikilink(ctx, (mark->ch != ']'),
                                 has_label ? STR(opener->beg+2) : STR(opener->end),
                                 target_sz));

                        break;
                    }

                    dest_mark = opener+1;
                    MD_ASSERT(dest_mark->ch == 'D');
                    title_mark = opener+2;
                    MD_ASSERT(title_mark->ch == 'D');

                    MD_CHECK(md_enter_leave_span_a(ctx, (mark->ch != ']'),
                                (opener->ch == '!' ? MD_SPAN_IMG : MD_SPAN_A),
                                STR(dest_mark->beg), dest_mark->end - dest_mark->beg, FALSE,
                                md_mark_get_ptr(ctx, title_mark - ctx->marks), title_mark->prev));

                    /* link/image closer may span multiple lines. */
                    if(mark->ch == ']') {
                        while(mark->end > line->end)
                            line++;
                    }

                    break;
                }

                case '<':
                case '>':       /* Autolink or raw HTML. */
                    if(!(mark->flags & MD_MARK_AUTOLINK)) {
                        /* Raw HTML. */
                        if(mark->flags & MD_MARK_OPENER)
                            text_type = MD_TEXT_HTML;
                        else
                            text_type = MD_TEXT_NORMAL;
                        break;
                    }
                    /* Pass through, if auto-link. */
                    MD_FALLTHROUGH();

                case '@':       /* Permissive e-mail autolink. */
                case ':':       /* Permissive URL autolink. */
                case '.':       /* Permissive WWW autolink. */
                {
                    MD_MARK* opener = ((mark->flags & MD_MARK_OPENER) ? mark : &ctx->marks[mark->prev]);
                    MD_MARK* closer = &ctx->marks[opener->next];
                    const CHAR* dest = STR(opener->end);
                    SZ dest_size = closer->beg - opener->end;

                    /* For permissive auto-links we do not know closer mark
                     * position at the time of md_collect_marks(), therefore
                     * it can be out-of-order in ctx->marks[].
                     *
                     * With this flag, we make sure that we output the closer
                     * only if we processed the opener. */
                    if(mark->flags & MD_MARK_OPENER)
                        closer->flags |= MD_MARK_VALIDPERMISSIVEAUTOLINK;

                    if(opener->ch == '@' || opener->ch == '.') {
                        dest_size += 7;
                        MD_TEMP_BUFFER(dest_size * sizeof(CHAR));
                        memcpy(ctx->buffer,
                                (opener->ch == '@' ? _T("mailto:") : _T("http://")),
                                7 * sizeof(CHAR));
                        memcpy(ctx->buffer + 7, dest, (dest_size-7) * sizeof(CHAR));
                        dest = ctx->buffer;
                    }

                    if(closer->flags & MD_MARK_VALIDPERMISSIVEAUTOLINK)
                        MD_CHECK(md_enter_leave_span_a(ctx, (mark->flags & MD_MARK_OPENER),
                                    MD_SPAN_A, dest, dest_size, TRUE, NULL, 0));
                    break;
                }

                case '&':       /* Entity. */
                    MD_TEXT(MD_TEXT_ENTITY, STR(mark->beg), mark->end - mark->beg);
                    break;

                case '\0':
                    MD_TEXT(MD_TEXT_NULLCHAR, _T(""), 1);
                    break;

                case 127:
                    goto abort;
            }

            off = mark->end;

            /* Move to next resolved mark. */
            prev_mark = mark;
            mark++;
            while(!(mark->flags & MD_MARK_RESOLVED)  ||  mark->beg < off)
                mark++;
        }

        /* If reached end of line, move to next one. */
        if(off >= line->end) {
            /* If it is the last line, we are done. */
            if(off >= end)
                break;

            if(text_type == MD_TEXT_CODE || text_type == MD_TEXT_LATEXMATH) {
                OFF tmp;

                MD_ASSERT(prev_mark != NULL);
                MD_ASSERT(ISANYOF2_(prev_mark->ch, '`', '$')  &&  (prev_mark->flags & MD_MARK_OPENER));
                MD_ASSERT(ISANYOF2_(mark->ch, '`', '$')  &&  (mark->flags & MD_MARK_CLOSER));

                /* Inside a code span, trailing line whitespace has to be
                 * outputted. */
                tmp = off;
                while(off < ctx->size  &&  ISBLANK(off))
                    off++;
                if(off > tmp)
                    MD_TEXT(text_type, STR(tmp), off-tmp);

                /* and new lines are transformed into single spaces. */
                if(prev_mark->end < off  &&  off < mark->beg)
                    MD_TEXT(text_type, _T(" "), 1);
            } else if(text_type == MD_TEXT_HTML) {
                /* Inside raw HTML, we output the new line verbatim, including
                 * any trailing spaces. */
                OFF tmp = off;

                while(tmp < end  &&  ISBLANK(tmp))
                    tmp++;
                if(tmp > off)
                    MD_TEXT(MD_TEXT_HTML, STR(off), tmp - off);
                MD_TEXT(MD_TEXT_HTML, _T("\n"), 1);
            } else {
                /* Output soft or hard line break. */
                MD_TEXTTYPE break_type = MD_TEXT_SOFTBR;

                if(text_type == MD_TEXT_NORMAL) {
                    if(enforce_hardbreak)
                        break_type = MD_TEXT_BR;
                    else if((CH(line->end) == _T(' ') && CH(line->end+1) == _T(' ')))
                        break_type = MD_TEXT_BR;
                }

                MD_TEXT(break_type, _T("\n"), 1);
            }

            /* Move to the next line. */
            line++;
            off = line->beg;

            enforce_hardbreak = 0;
        }
    }

abort:
    return ret;
}