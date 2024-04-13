md_analyze_entity(MD_CTX* ctx, int mark_index)
{
    MD_MARK* opener = &ctx->marks[mark_index];
    MD_MARK* closer;
    OFF off;

    /* Cannot be entity if there is no closer as the next mark.
     * (Any other mark between would mean strange character which cannot be
     * part of the entity.
     *
     * So we can do all the work on '&' and do not call this later for the
     * closing mark ';'.
     */
    if(mark_index + 1 >= ctx->n_marks)
        return;
    closer = &ctx->marks[mark_index+1];
    if(closer->ch != ';')
        return;

    if(md_is_entity(ctx, opener->beg, closer->end, &off)) {
        MD_ASSERT(off == closer->end);

        md_resolve_range(ctx, NULL, mark_index, mark_index+1);
        opener->end = closer->end;
    }
}