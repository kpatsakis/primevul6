md_process_normal_block_contents(MD_CTX* ctx, const MD_LINE* lines, int n_lines)
{
    int i;
    int ret;

    MD_CHECK(md_analyze_inlines(ctx, lines, n_lines, FALSE));
    MD_CHECK(md_process_inlines(ctx, lines, n_lines));

abort:
    /* Free any temporary memory blocks stored within some dummy marks. */
    for(i = PTR_CHAIN.head; i >= 0; i = ctx->marks[i].next)
        free(md_mark_get_ptr(ctx, i));
    PTR_CHAIN.head = -1;
    PTR_CHAIN.tail = -1;

    return ret;
}