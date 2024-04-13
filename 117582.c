md_analyze_tilde(MD_CTX* ctx, int mark_index)
{
    MD_MARK* mark = &ctx->marks[mark_index];
    MD_MARKCHAIN* chain = md_mark_chain(ctx, mark_index);

    /* We attempt to be Github Flavored Markdown compatible here. GFM accepts
     * only tildes sequences of length 1 and 2, and the length of the opener
     * and closer has to match. */

    if((mark->flags & MD_MARK_POTENTIAL_CLOSER)  &&  chain->head >= 0) {
        int opener_index = chain->head;

        md_rollback(ctx, opener_index, mark_index, MD_ROLLBACK_CROSSING);
        md_resolve_range(ctx, chain, opener_index, mark_index);
        return;
    }

    if(mark->flags & MD_MARK_POTENTIAL_OPENER)
        md_mark_chain_append(ctx, chain, mark_index);
}