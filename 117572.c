md_process_table_row(MD_CTX* ctx, MD_BLOCKTYPE cell_type, OFF beg, OFF end,
                     const MD_ALIGN* align, int col_count)
{
    MD_LINE line;
    OFF* pipe_offs = NULL;
    int i, j, k, n;
    int ret = 0;

    line.beg = beg;
    line.end = end;

    /* Break the line into table cells by identifying pipe characters who
     * form the cell boundary. */
    MD_CHECK(md_analyze_inlines(ctx, &line, 1, TRUE));

    /* We have to remember the cell boundaries in local buffer because
     * ctx->marks[] shall be reused during cell contents processing. */
    n = ctx->n_table_cell_boundaries + 2;
    pipe_offs = (OFF*) malloc(n * sizeof(OFF));
    if(pipe_offs == NULL) {
        MD_LOG("malloc() failed.");
        ret = -1;
        goto abort;
    }
    j = 0;
    pipe_offs[j++] = beg;
    for(i = TABLECELLBOUNDARIES.head; i >= 0; i = ctx->marks[i].next) {
        MD_MARK* mark = &ctx->marks[i];
        pipe_offs[j++] = mark->end;
    }
    pipe_offs[j++] = end+1;

    /* Process cells. */
    MD_ENTER_BLOCK(MD_BLOCK_TR, NULL);
    k = 0;
    for(i = 0; i < j-1  &&  k < col_count; i++) {
        if(pipe_offs[i] < pipe_offs[i+1]-1)
            MD_CHECK(md_process_table_cell(ctx, cell_type, align[k++], pipe_offs[i], pipe_offs[i+1]-1));
    }
    /* Make sure we call enough table cells even if the current table contains
     * too few of them. */
    while(k < col_count)
        MD_CHECK(md_process_table_cell(ctx, cell_type, align[k++], 0, 0));
    MD_LEAVE_BLOCK(MD_BLOCK_TR, NULL);

abort:
    free(pipe_offs);

    /* Free any temporary memory blocks stored within some dummy marks. */
    for(i = PTR_CHAIN.head; i >= 0; i = ctx->marks[i].next)
        free(md_mark_get_ptr(ctx, i));
    PTR_CHAIN.head = -1;
    PTR_CHAIN.tail = -1;

    return ret;
}