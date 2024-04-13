md_process_table_cell(MD_CTX* ctx, MD_BLOCKTYPE cell_type, MD_ALIGN align, OFF beg, OFF end)
{
    MD_LINE line;
    MD_BLOCK_TD_DETAIL det;
    int ret = 0;

    while(beg < end  &&  ISWHITESPACE(beg))
        beg++;
    while(end > beg  &&  ISWHITESPACE(end-1))
        end--;

    det.align = align;
    line.beg = beg;
    line.end = end;

    MD_ENTER_BLOCK(cell_type, &det);
    MD_CHECK(md_process_normal_block_contents(ctx, &line, 1));
    MD_LEAVE_BLOCK(cell_type, &det);

abort:
    return ret;
}