md_analyze_permissive_url_autolink(MD_CTX* ctx, int mark_index)
{
    MD_MARK* opener = &ctx->marks[mark_index];
    int closer_index = mark_index + 1;
    MD_MARK* closer = &ctx->marks[closer_index];
    MD_MARK* next_resolved_mark;
    OFF off = opener->end;
    int n_dots = FALSE;
    int has_underscore_in_last_seg = FALSE;
    int has_underscore_in_next_to_last_seg = FALSE;
    int n_opened_parenthesis = 0;
    int n_excess_parenthesis = 0;

    /* Check for domain. */
    while(off < ctx->size) {
        if(ISALNUM(off) || CH(off) == _T('-')) {
            off++;
        } else if(CH(off) == _T('.')) {
            /* We must see at least one period. */
            n_dots++;
            has_underscore_in_next_to_last_seg = has_underscore_in_last_seg;
            has_underscore_in_last_seg = FALSE;
            off++;
        } else if(CH(off) == _T('_')) {
            /* No underscore may be present in the last two domain segments. */
            has_underscore_in_last_seg = TRUE;
            off++;
        } else {
            break;
        }
    }
    if(off > opener->end  &&  CH(off-1) == _T('.')) {
        off--;
        n_dots--;
    }
    if(off <= opener->end || n_dots == 0 || has_underscore_in_next_to_last_seg || has_underscore_in_last_seg)
        return;

    /* Check for path. */
    next_resolved_mark = closer + 1;
    while(next_resolved_mark->ch == 'D' || !(next_resolved_mark->flags & MD_MARK_RESOLVED))
        next_resolved_mark++;
    while(off < next_resolved_mark->beg  &&  CH(off) != _T('<')  &&  !ISWHITESPACE(off)  &&  !ISNEWLINE(off)) {
        /* Parenthesis must be balanced. */
        if(CH(off) == _T('(')) {
            n_opened_parenthesis++;
        } else if(CH(off) == _T(')')) {
            if(n_opened_parenthesis > 0)
                n_opened_parenthesis--;
            else
                n_excess_parenthesis++;
        }

        off++;
    }

    /* Trim a trailing punctuation from the end. */
    while(TRUE) {
        if(ISANYOF(off-1, _T("?!.,:*_~"))) {
            off--;
        } else if(CH(off-1) == ')'  &&  n_excess_parenthesis > 0) {
            /* Unmatched ')' can be in an interior of the path but not at the
             * of it, so the auto-link may be safely nested in a parenthesis
             * pair. */
            off--;
            n_excess_parenthesis--;
        } else {
            break;
        }
    }

    /* Ok. Lets call it an auto-link. Adapt opener and create closer to zero
     * length so all the contents becomes the link text. */
    MD_ASSERT(closer->ch == 'D');
    opener->end = opener->beg;
    closer->ch = opener->ch;
    closer->beg = off;
    closer->end = off;
    md_resolve_range(ctx, NULL, mark_index, closer_index);
}