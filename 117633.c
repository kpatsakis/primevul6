md_analyze_bracket(MD_CTX* ctx, int mark_index)
{
    /* We cannot really resolve links here as for that we would need
     * more context. E.g. a following pair of brackets (reference link),
     * or enclosing pair of brackets (if the inner is the link, the outer
     * one cannot be.)
     *
     * Therefore we here only construct a list of resolved '[' ']' pairs
     * ordered by position of the closer. This allows ur to analyze what is
     * or is not link in the right order, from inside to outside in case
     * of nested brackets.
     *
     * The resolving itself is deferred into md_resolve_links().
     */

    MD_MARK* mark = &ctx->marks[mark_index];

    if(mark->flags & MD_MARK_POTENTIAL_OPENER) {
        md_mark_chain_append(ctx, &BRACKET_OPENERS, mark_index);
        return;
    }

    if(BRACKET_OPENERS.tail >= 0) {
        /* Pop the opener from the chain. */
        int opener_index = BRACKET_OPENERS.tail;
        MD_MARK* opener = &ctx->marks[opener_index];
        if(opener->prev >= 0)
            ctx->marks[opener->prev].next = -1;
        else
            BRACKET_OPENERS.head = -1;
        BRACKET_OPENERS.tail = opener->prev;

        /* Interconnect the opener and closer. */
        opener->next = mark_index;
        mark->prev = opener_index;

        /* Add the pair into chain of potential links for md_resolve_links().
         * Note we misuse opener->prev for this as opener->next points to its
         * closer. */
        if(ctx->unresolved_link_tail >= 0)
            ctx->marks[ctx->unresolved_link_tail].prev = opener_index;
        else
            ctx->unresolved_link_head = opener_index;
        ctx->unresolved_link_tail = opener_index;
        opener->prev = -1;
    }
}