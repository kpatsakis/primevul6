md_resolve_links(MD_CTX* ctx, const MD_LINE* lines, int n_lines)
{
    int opener_index = ctx->unresolved_link_head;
    OFF last_link_beg = 0;
    OFF last_link_end = 0;
    OFF last_img_beg = 0;
    OFF last_img_end = 0;

    while(opener_index >= 0) {
        MD_MARK* opener = &ctx->marks[opener_index];
        int closer_index = opener->next;
        MD_MARK* closer = &ctx->marks[closer_index];
        int next_index = opener->prev;
        MD_MARK* next_opener;
        MD_MARK* next_closer;
        MD_LINK_ATTR attr;
        int is_link = FALSE;

        if(next_index >= 0) {
            next_opener = &ctx->marks[next_index];
            next_closer = &ctx->marks[next_opener->next];
        } else {
            next_opener = NULL;
            next_closer = NULL;
        }

        /* If nested ("[ [ ] ]"), we need to make sure that:
         *   - The outer does not end inside of (...) belonging to the inner.
         *   - The outer cannot be link if the inner is link (i.e. not image).
         *
         * (Note we here analyze from inner to outer as the marks are ordered
         * by closer->beg.)
         */
        if((opener->beg < last_link_beg  &&  closer->end < last_link_end)  ||
           (opener->beg < last_img_beg  &&  closer->end < last_img_end)  ||
           (opener->beg < last_link_end  &&  opener->ch == '['))
        {
            opener_index = next_index;
            continue;
        }

        /* Recognize and resolve wiki links.
         * Wiki-links maybe '[[destination]]' or '[[destination|label]]'.
         */
        if ((ctx->parser.flags & MD_FLAG_WIKILINKS) &&
            (opener->end - opener->beg == 1) &&         /* not image */
            next_opener != NULL &&                      /* double '[' opener */
            next_opener->ch == '[' &&
            (next_opener->beg == opener->beg - 1) &&
            (next_opener->end - next_opener->beg == 1) &&
            next_closer != NULL &&                      /* double ']' closer */
            next_closer->ch == ']' &&
            (next_closer->beg == closer->beg + 1) &&
            (next_closer->end - next_closer->beg == 1))
        {
            MD_MARK* delim = NULL;
            int delim_index;
            OFF dest_beg, dest_end;

            is_link = TRUE;

            /* We don't allow destination to be longer than 100 characters.
             * Lets scan to see whether there is '|'. (If not then the whole
             * wiki-link has to be below the 100 characters.) */
            delim_index = opener_index + 1;
            while(delim_index < closer_index) {
                MD_MARK* m = &ctx->marks[delim_index];
                if(m->ch == '|') {
                    delim = m;
                    break;
                }
                if(m->ch != 'D'  &&  m->beg - opener->end > 100)
                    break;
                delim_index++;
            }
            dest_beg = opener->end;
            dest_end = (delim != NULL) ? delim->beg : closer->beg;
            if(dest_end - dest_beg == 0 || dest_end - dest_beg > 100)
                is_link = FALSE;

            /* There may not be any new line in the destination. */
            if(is_link) {
                OFF off;
                for(off = dest_beg; off < dest_end; off++) {
                    if(ISNEWLINE(off)) {
                        is_link = FALSE;
                        break;
                    }
                }
            }

            if(is_link) {
                if(delim != NULL) {
                    if(delim->end < closer->beg) {
                        opener->end = delim->beg;
                    } else {
                        /* The pipe is just before the closer: [[foo|]] */
                        closer->beg = delim->beg;
                        delim = NULL;
                    }
                }

                opener->beg = next_opener->beg;
                opener->next = closer_index;
                opener->flags |= MD_MARK_OPENER | MD_MARK_RESOLVED;

                closer->end = next_closer->end;
                closer->prev = opener_index;
                closer->flags |= MD_MARK_CLOSER | MD_MARK_RESOLVED;

                last_link_beg = opener->beg;
                last_link_end = closer->end;

                if(delim != NULL) {
                    delim->flags |= MD_MARK_RESOLVED;
                    md_rollback(ctx, opener_index, delim_index, MD_ROLLBACK_ALL);
                    md_analyze_link_contents(ctx, lines, n_lines, opener_index+1, closer_index);
                } else {
                    md_rollback(ctx, opener_index, closer_index, MD_ROLLBACK_ALL);
                }

                opener_index = next_opener->prev;
                continue;
            }
        }

        if(next_opener != NULL  &&  next_opener->beg == closer->end) {
            if(next_closer->beg > closer->end + 1) {
                /* Might be full reference link. */
                is_link = md_is_link_reference(ctx, lines, n_lines, next_opener->beg, next_closer->end, &attr);
            } else {
                /* Might be shortcut reference link. */
                is_link = md_is_link_reference(ctx, lines, n_lines, opener->beg, closer->end, &attr);
            }

            if(is_link < 0)
                return -1;

            if(is_link) {
                /* Eat the 2nd "[...]". */
                closer->end = next_closer->end;

                /* Do not analyze the label as a standalone link in the next
                 * iteration. */
                next_index = ctx->marks[next_index].prev;
            }
        } else {
            if(closer->end < ctx->size  &&  CH(closer->end) == _T('(')) {
                /* Might be inline link. */
                OFF inline_link_end = UINT_MAX;

                is_link = md_is_inline_link_spec(ctx, lines, n_lines, closer->end, &inline_link_end, &attr);
                if(is_link < 0)
                    return -1;

                /* Check the closing ')' is not inside an already resolved range
                 * (i.e. a range with a higher priority), e.g. a code span. */
                if(is_link) {
                    int i = closer_index + 1;

                    while(i < ctx->n_marks) {
                        MD_MARK* mark = &ctx->marks[i];

                        if(mark->beg >= inline_link_end)
                            break;
                        if((mark->flags & (MD_MARK_OPENER | MD_MARK_RESOLVED)) == (MD_MARK_OPENER | MD_MARK_RESOLVED)) {
                            if(ctx->marks[mark->next].beg >= inline_link_end) {
                                /* Cancel the link status. */
                                if(attr.title_needs_free)
                                    free(attr.title);
                                is_link = FALSE;
                                break;
                            }

                            i = mark->next + 1;
                        } else {
                            i++;
                        }
                    }
                }

                if(is_link) {
                    /* Eat the "(...)" */
                    closer->end = inline_link_end;
                }
            }

            if(!is_link) {
                /* Might be collapsed reference link. */
                is_link = md_is_link_reference(ctx, lines, n_lines, opener->beg, closer->end, &attr);
                if(is_link < 0)
                    return -1;
            }
        }

        if(is_link) {
            /* Resolve the brackets as a link. */
            opener->flags |= MD_MARK_OPENER | MD_MARK_RESOLVED;
            closer->flags |= MD_MARK_CLOSER | MD_MARK_RESOLVED;

            /* If it is a link, we store the destination and title in the two
             * dummy marks after the opener. */
            MD_ASSERT(ctx->marks[opener_index+1].ch == 'D');
            ctx->marks[opener_index+1].beg = attr.dest_beg;
            ctx->marks[opener_index+1].end = attr.dest_end;

            MD_ASSERT(ctx->marks[opener_index+2].ch == 'D');
            md_mark_store_ptr(ctx, opener_index+2, attr.title);
            /* The title might or might not have been allocated for us. */
            if(attr.title_needs_free)
                md_mark_chain_append(ctx, &PTR_CHAIN, opener_index+2);
            ctx->marks[opener_index+2].prev = attr.title_size;

            if(opener->ch == '[') {
                last_link_beg = opener->beg;
                last_link_end = closer->end;
            } else {
                last_img_beg = opener->beg;
                last_img_end = closer->end;
            }

            md_analyze_link_contents(ctx, lines, n_lines, opener_index+1, closer_index);
        }

        opener_index = next_index;
    }

    return 0;
}