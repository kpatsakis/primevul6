md_process_leaf_block(MD_CTX* ctx, const MD_BLOCK* block)
{
    union {
        MD_BLOCK_H_DETAIL header;
        MD_BLOCK_CODE_DETAIL code;
        MD_BLOCK_TABLE_DETAIL table;
    } det;
    MD_ATTRIBUTE_BUILD info_build;
    MD_ATTRIBUTE_BUILD lang_build;
    int is_in_tight_list;
    int clean_fence_code_detail = FALSE;
    int ret = 0;

    memset(&det, 0, sizeof(det));

    if(ctx->n_containers == 0)
        is_in_tight_list = FALSE;
    else
        is_in_tight_list = !ctx->containers[ctx->n_containers-1].is_loose;

    switch(block->type) {
        case MD_BLOCK_H:
            det.header.level = block->data;
            break;

        case MD_BLOCK_CODE:
            /* For fenced code block, we may need to set the info string. */
            if(block->data != 0) {
                memset(&det.code, 0, sizeof(MD_BLOCK_CODE_DETAIL));
                clean_fence_code_detail = TRUE;
                MD_CHECK(md_setup_fenced_code_detail(ctx, block, &det.code, &info_build, &lang_build));
            }
            break;

        case MD_BLOCK_TABLE:
            det.table.col_count = block->data;
            det.table.head_row_count = 1;
            det.table.body_row_count = block->n_lines - 2;
            break;

        default:
            /* Noop. */
            break;
    }

    if(!is_in_tight_list  ||  block->type != MD_BLOCK_P)
        MD_ENTER_BLOCK(block->type, (void*) &det);

    /* Process the block contents accordingly to is type. */
    switch(block->type) {
        case MD_BLOCK_HR:
            /* noop */
            break;

        case MD_BLOCK_CODE:
            MD_CHECK(md_process_code_block_contents(ctx, (block->data != 0),
                            (const MD_VERBATIMLINE*)(block + 1), block->n_lines));
            break;

        case MD_BLOCK_HTML:
            MD_CHECK(md_process_verbatim_block_contents(ctx, MD_TEXT_HTML,
                            (const MD_VERBATIMLINE*)(block + 1), block->n_lines));
            break;

        case MD_BLOCK_TABLE:
            MD_CHECK(md_process_table_block_contents(ctx, block->data,
                            (const MD_LINE*)(block + 1), block->n_lines));
            break;

        default:
            MD_CHECK(md_process_normal_block_contents(ctx,
                            (const MD_LINE*)(block + 1), block->n_lines));
            break;
    }

    if(!is_in_tight_list  ||  block->type != MD_BLOCK_P)
        MD_LEAVE_BLOCK(block->type, (void*) &det);

abort:
    if(clean_fence_code_detail) {
        md_free_attribute(ctx, &info_build);
        md_free_attribute(ctx, &lang_build);
    }
    return ret;
}