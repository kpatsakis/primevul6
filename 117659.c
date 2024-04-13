md_process_table_block_contents(MD_CTX* ctx, int col_count, const MD_LINE* lines, int n_lines)
{
    MD_ALIGN* align;
    int i;
    int ret = 0;

    /* At least two lines have to be present: The column headers and the line
     * with the underlines. */
    MD_ASSERT(n_lines >= 2);

    align = malloc(col_count * sizeof(MD_ALIGN));
    if(align == NULL) {
        MD_LOG("malloc() failed.");
        ret = -1;
        goto abort;
    }

    md_analyze_table_alignment(ctx, lines[1].beg, lines[1].end, align, col_count);

    MD_ENTER_BLOCK(MD_BLOCK_THEAD, NULL);
    MD_CHECK(md_process_table_row(ctx, MD_BLOCK_TH,
                        lines[0].beg, lines[0].end, align, col_count));
    MD_LEAVE_BLOCK(MD_BLOCK_THEAD, NULL);

    if(n_lines > 2) {
        MD_ENTER_BLOCK(MD_BLOCK_TBODY, NULL);
        for(i = 2; i < n_lines; i++) {
            MD_CHECK(md_process_table_row(ctx, MD_BLOCK_TD,
                     lines[i].beg, lines[i].end, align, col_count));
        }
        MD_LEAVE_BLOCK(MD_BLOCK_TBODY, NULL);
    }

abort:
    free(align);
    return ret;
}