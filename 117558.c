md_analyze_dollar(MD_CTX* ctx, int mark_index)
{
    /* This should mimic the way inline equations work in LaTeX, so there
     * can only ever be one item in the chain (i.e. the dollars can't be
     * nested). This is basically the same as the md_analyze_tilde function,
     * except that we require matching openers and closers to be of the same
     * length.
     *
     * E.g.: $abc$$def$$ => abc (display equation) def (end equation) */
    if(DOLLAR_OPENERS.head >= 0) {
        /* If the potential closer has a non-matching number of $, discard */
        MD_MARK* open = &ctx->marks[DOLLAR_OPENERS.head];
        MD_MARK* close = &ctx->marks[mark_index];

        int opener_index = DOLLAR_OPENERS.head;
        md_rollback(ctx, opener_index, mark_index, MD_ROLLBACK_ALL);
        if (open->end - open->beg == close->end - close->beg) {
            /* We are the matching closer */
            md_resolve_range(ctx, &DOLLAR_OPENERS, opener_index, mark_index);
        } else {
            /* We don't match the opener, so discard old opener and insert as opener */
            md_mark_chain_append(ctx, &DOLLAR_OPENERS, mark_index);
        }
    } else {
        /* No unmatched openers, so we are opener */
        md_mark_chain_append(ctx, &DOLLAR_OPENERS, mark_index);
    }
}