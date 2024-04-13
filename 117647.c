md_collect_marks(MD_CTX* ctx, const MD_LINE* lines, int n_lines, int table_mode)
{
    int i;
    int ret = 0;
    MD_MARK* mark;
    OFF codespan_last_potential_closers[CODESPAN_MARK_MAXLEN] = { 0 };
    int codespan_scanned_till_paragraph_end = FALSE;

    for(i = 0; i < n_lines; i++) {
        const MD_LINE* line = &lines[i];
        OFF off = line->beg;
        OFF line_end = line->end;

        while(TRUE) {
            CHAR ch;

#ifdef MD4C_USE_UTF16
    /* For UTF-16, mark_char_map[] covers only ASCII. */
    #define IS_MARK_CHAR(off)   ((CH(off) < SIZEOF_ARRAY(ctx->mark_char_map))  &&  \
                                (ctx->mark_char_map[(unsigned char) CH(off)]))
#else
    /* For 8-bit encodings, mark_char_map[] covers all 256 elements. */
    #define IS_MARK_CHAR(off)   (ctx->mark_char_map[(unsigned char) CH(off)])
#endif

            /* Optimization: Use some loop unrolling. */
            while(off + 3 < line_end  &&  !IS_MARK_CHAR(off+0)  &&  !IS_MARK_CHAR(off+1)
                                      &&  !IS_MARK_CHAR(off+2)  &&  !IS_MARK_CHAR(off+3))
                off += 4;
            while(off < line_end  &&  !IS_MARK_CHAR(off+0))
                off++;

            if(off >= line_end)
                break;

            ch = CH(off);

            /* A backslash escape.
             * It can go beyond line->end as it may involve escaped new
             * line to form a hard break. */
            if(ch == _T('\\')  &&  off+1 < ctx->size  &&  (ISPUNCT(off+1) || ISNEWLINE(off+1))) {
                /* Hard-break cannot be on the last line of the block. */
                if(!ISNEWLINE(off+1)  ||  i+1 < n_lines)
                    PUSH_MARK(ch, off, off+2, MD_MARK_RESOLVED);
                off += 2;
                continue;
            }

            /* A potential (string) emphasis start/end. */
            if(ch == _T('*')  ||  ch == _T('_')) {
                OFF tmp = off+1;
                int left_level;     /* What precedes: 0 = whitespace; 1 = punctuation; 2 = other char. */
                int right_level;    /* What follows: 0 = whitespace; 1 = punctuation; 2 = other char. */

                while(tmp < line_end  &&  CH(tmp) == ch)
                    tmp++;

                if(off == line->beg  ||  ISUNICODEWHITESPACEBEFORE(off))
                    left_level = 0;
                else if(ISUNICODEPUNCTBEFORE(off))
                    left_level = 1;
                else
                    left_level = 2;

                if(tmp == line_end  ||  ISUNICODEWHITESPACE(tmp))
                    right_level = 0;
                else if(ISUNICODEPUNCT(tmp))
                    right_level = 1;
                else
                    right_level = 2;

                /* Intra-word underscore doesn't have special meaning. */
                if(ch == _T('_')  &&  left_level == 2  &&  right_level == 2) {
                    left_level = 0;
                    right_level = 0;
                }

                if(left_level != 0  ||  right_level != 0) {
                    unsigned flags = 0;

                    if(left_level > 0  &&  left_level >= right_level)
                        flags |= MD_MARK_POTENTIAL_CLOSER;
                    if(right_level > 0  &&  right_level >= left_level)
                        flags |= MD_MARK_POTENTIAL_OPENER;
                    if(left_level == 2  &&  right_level == 2)
                        flags |= MD_MARK_EMPH_INTRAWORD;

                    /* For "the rule of three" we need to remember the original
                     * size of the mark (modulo three), before we potentially
                     * split the mark when being later resolved partially by some
                     * shorter closer. */
                    switch((tmp - off) % 3) {
                        case 0: flags |= MD_MARK_EMPH_MOD3_0; break;
                        case 1: flags |= MD_MARK_EMPH_MOD3_1; break;
                        case 2: flags |= MD_MARK_EMPH_MOD3_2; break;
                    }

                    PUSH_MARK(ch, off, tmp, flags);

                    /* During resolving, multiple asterisks may have to be
                     * split into independent span start/ends. Consider e.g.
                     * "**foo* bar*". Therefore we push also some empty dummy
                     * marks to have enough space for that. */
                    off++;
                    while(off < tmp) {
                        PUSH_MARK('D', off, off, 0);
                        off++;
                    }
                    continue;
                }

                off = tmp;
                continue;
            }

            /* A potential code span start/end. */
            if(ch == _T('`')) {
                OFF opener_beg, opener_end;
                OFF closer_beg, closer_end;
                int is_code_span;

                is_code_span = md_is_code_span(ctx, lines + i, n_lines - i, off,
                                    &opener_beg, &opener_end, &closer_beg, &closer_end,
                                    codespan_last_potential_closers,
                                    &codespan_scanned_till_paragraph_end);
                if(is_code_span) {
                    PUSH_MARK(_T('`'), opener_beg, opener_end, MD_MARK_OPENER | MD_MARK_RESOLVED);
                    PUSH_MARK(_T('`'), closer_beg, closer_end, MD_MARK_CLOSER | MD_MARK_RESOLVED);
                    ctx->marks[ctx->n_marks-2].next = ctx->n_marks-1;
                    ctx->marks[ctx->n_marks-1].prev = ctx->n_marks-2;

                    off = closer_end;

                    /* Advance the current line accordingly. */
                    while(off > line_end) {
                        i++;
                        line++;
                        line_end = line->end;
                    }
                    continue;
                }

                off = opener_end;
                continue;
            }

            /* A potential entity start. */
            if(ch == _T('&')) {
                PUSH_MARK(ch, off, off+1, MD_MARK_POTENTIAL_OPENER);
                off++;
                continue;
            }

            /* A potential entity end. */
            if(ch == _T(';')) {
                /* We surely cannot be entity unless the previous mark is '&'. */
                if(ctx->n_marks > 0  &&  ctx->marks[ctx->n_marks-1].ch == _T('&'))
                    PUSH_MARK(ch, off, off+1, MD_MARK_POTENTIAL_CLOSER);

                off++;
                continue;
            }

            /* A potential autolink or raw HTML start/end. */
            if(ch == _T('<')) {
                int is_autolink;
                OFF autolink_end;
                int missing_mailto;

                if(!(ctx->parser.flags & MD_FLAG_NOHTMLSPANS)) {
                    int is_html;
                    OFF html_end;

                    /* Given the nature of the raw HTML, we have to recognize
                     * it here. Doing so later in md_analyze_lt_gt() could
                     * open can of worms of quadratic complexity. */
                    is_html = md_is_html_any(ctx, lines + i, n_lines - i, off,
                                    lines[n_lines-1].end, &html_end);
                    if(is_html) {
                        PUSH_MARK(_T('<'), off, off, MD_MARK_OPENER | MD_MARK_RESOLVED);
                        PUSH_MARK(_T('>'), html_end, html_end, MD_MARK_CLOSER | MD_MARK_RESOLVED);
                        ctx->marks[ctx->n_marks-2].next = ctx->n_marks-1;
                        ctx->marks[ctx->n_marks-1].prev = ctx->n_marks-2;
                        off = html_end;

                        /* Advance the current line accordingly. */
                        while(off > line_end) {
                            i++;
                            line++;
                            line_end = line->end;
                        }
                        continue;
                    }
                }

                is_autolink = md_is_autolink(ctx, off, lines[n_lines-1].end,
                                    &autolink_end, &missing_mailto);
                if(is_autolink) {
                    PUSH_MARK((missing_mailto ? _T('@') : _T('<')), off, off+1,
                                MD_MARK_OPENER | MD_MARK_RESOLVED | MD_MARK_AUTOLINK);
                    PUSH_MARK(_T('>'), autolink_end-1, autolink_end,
                                MD_MARK_CLOSER | MD_MARK_RESOLVED | MD_MARK_AUTOLINK);
                    ctx->marks[ctx->n_marks-2].next = ctx->n_marks-1;
                    ctx->marks[ctx->n_marks-1].prev = ctx->n_marks-2;
                    off = autolink_end;
                    continue;
                }

                off++;
                continue;
            }

            /* A potential link or its part. */
            if(ch == _T('[')  ||  (ch == _T('!') && off+1 < line_end && CH(off+1) == _T('['))) {
                OFF tmp = (ch == _T('[') ? off+1 : off+2);
                PUSH_MARK(ch, off, tmp, MD_MARK_POTENTIAL_OPENER);
                off = tmp;
                /* Two dummies to make enough place for data we need if it is
                 * a link. */
                PUSH_MARK('D', off, off, 0);
                PUSH_MARK('D', off, off, 0);
                continue;
            }
            if(ch == _T(']')) {
                PUSH_MARK(ch, off, off+1, MD_MARK_POTENTIAL_CLOSER);
                off++;
                continue;
            }

            /* A potential permissive e-mail autolink. */
            if(ch == _T('@')) {
                if(line->beg + 1 <= off  &&  ISALNUM(off-1)  &&
                    off + 3 < line->end  &&  ISALNUM(off+1))
                {
                    PUSH_MARK(ch, off, off+1, MD_MARK_POTENTIAL_OPENER);
                    /* Push a dummy as a reserve for a closer. */
                    PUSH_MARK('D', off, off, 0);
                }

                off++;
                continue;
            }

            /* A potential permissive URL autolink. */
            if(ch == _T(':')) {
                static struct {
                    const CHAR* scheme;
                    SZ scheme_size;
                    const CHAR* suffix;
                    SZ suffix_size;
                } scheme_map[] = {
                    /* In the order from the most frequently used, arguably. */
                    { _T("http"), 4,    _T("//"), 2 },
                    { _T("https"), 5,   _T("//"), 2 },
                    { _T("ftp"), 3,     _T("//"), 2 }
                };
                int scheme_index;

                for(scheme_index = 0; scheme_index < (int) SIZEOF_ARRAY(scheme_map); scheme_index++) {
                    const CHAR* scheme = scheme_map[scheme_index].scheme;
                    const SZ scheme_size = scheme_map[scheme_index].scheme_size;
                    const CHAR* suffix = scheme_map[scheme_index].suffix;
                    const SZ suffix_size = scheme_map[scheme_index].suffix_size;

                    if(line->beg + scheme_size <= off  &&  md_ascii_eq(STR(off-scheme_size), scheme, scheme_size)  &&
                        (line->beg + scheme_size == off || ISWHITESPACE(off-scheme_size-1) || ISANYOF(off-scheme_size-1, _T("*_~([")))  &&
                        off + 1 + suffix_size < line->end  &&  md_ascii_eq(STR(off+1), suffix, suffix_size))
                    {
                        PUSH_MARK(ch, off-scheme_size, off+1+suffix_size, MD_MARK_POTENTIAL_OPENER);
                        /* Push a dummy as a reserve for a closer. */
                        PUSH_MARK('D', off, off, 0);
                        off += 1 + suffix_size;
                        break;
                    }
                }

                off++;
                continue;
            }

            /* A potential permissive WWW autolink. */
            if(ch == _T('.')) {
                if(line->beg + 3 <= off  &&  md_ascii_eq(STR(off-3), _T("www"), 3)  &&
                    (line->beg + 3 == off || ISWHITESPACE(off-4) || ISANYOF(off-4, _T("*_~([")))  &&
                    off + 1 < line_end)
                {
                    PUSH_MARK(ch, off-3, off+1, MD_MARK_POTENTIAL_OPENER);
                    /* Push a dummy as a reserve for a closer. */
                    PUSH_MARK('D', off, off, 0);
                    off++;
                    continue;
                }

                off++;
                continue;
            }

            /* A potential table cell boundary or wiki link label delimiter. */
            if((table_mode || ctx->parser.flags & MD_FLAG_WIKILINKS) && ch == _T('|')) {
                PUSH_MARK(ch, off, off+1, 0);
                off++;
                continue;
            }

            /* A potential strikethrough start/end. */
            if(ch == _T('~')) {
                OFF tmp = off+1;

                while(tmp < line_end  &&  CH(tmp) == _T('~'))
                    tmp++;

                if(tmp - off < 3) {
                    unsigned flags = 0;

                    if(tmp < line_end  &&  !ISUNICODEWHITESPACE(tmp))
                        flags |= MD_MARK_POTENTIAL_OPENER;
                    if(off > line->beg  &&  !ISUNICODEWHITESPACEBEFORE(off))
                        flags |= MD_MARK_POTENTIAL_CLOSER;
                    if(flags != 0)
                        PUSH_MARK(ch, off, tmp, flags);
                }

                off = tmp;
                continue;
            }

            /* A potential equation start/end */
            if(ch == _T('$')) {
                /* We can have at most two consecutive $ signs,
                 * where two dollar signs signify a display equation. */
                OFF tmp = off+1;

                while(tmp < line_end && CH(tmp) == _T('$'))
                    tmp++;

                if (tmp - off <= 2)
                    PUSH_MARK(ch, off, tmp, MD_MARK_POTENTIAL_OPENER | MD_MARK_POTENTIAL_CLOSER);
                off = tmp;
                continue;
            }

            /* Turn non-trivial whitespace into single space. */
            if(ISWHITESPACE_(ch)) {
                OFF tmp = off+1;

                while(tmp < line_end  &&  ISWHITESPACE(tmp))
                    tmp++;

                if(tmp - off > 1  ||  ch != _T(' '))
                    PUSH_MARK(ch, off, tmp, MD_MARK_RESOLVED);

                off = tmp;
                continue;
            }

            /* NULL character. */
            if(ch == _T('\0')) {
                PUSH_MARK(ch, off, off+1, MD_MARK_RESOLVED);
                off++;
                continue;
            }

            off++;
        }
    }

    /* Add a dummy mark at the end of the mark vector to simplify
     * process_inlines(). */
    PUSH_MARK(127, ctx->size, ctx->size, MD_MARK_RESOLVED);

abort:
    return ret;
}