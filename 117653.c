md_process_all_blocks(MD_CTX* ctx)
{
    int byte_off = 0;
    int ret = 0;

    /* ctx->containers now is not needed for detection of lists and list items
     * so we reuse it for tracking what lists are loose or tight. We rely
     * on the fact the vector is large enough to hold the deepest nesting
     * level of lists. */
    ctx->n_containers = 0;

    while(byte_off < ctx->n_block_bytes) {
        MD_BLOCK* block = (MD_BLOCK*)((char*)ctx->block_bytes + byte_off);
        union {
            MD_BLOCK_UL_DETAIL ul;
            MD_BLOCK_OL_DETAIL ol;
            MD_BLOCK_LI_DETAIL li;
        } det;

        switch(block->type) {
            case MD_BLOCK_UL:
                det.ul.is_tight = (block->flags & MD_BLOCK_LOOSE_LIST) ? FALSE : TRUE;
                det.ul.mark = (CHAR) block->data;
                break;

            case MD_BLOCK_OL:
                det.ol.start = block->n_lines;
                det.ol.is_tight =  (block->flags & MD_BLOCK_LOOSE_LIST) ? FALSE : TRUE;
                det.ol.mark_delimiter = (CHAR) block->data;
                break;

            case MD_BLOCK_LI:
                det.li.is_task = (block->data != 0);
                det.li.task_mark = (CHAR) block->data;
                det.li.task_mark_offset = (OFF) block->n_lines;
                break;

            default:
                /* noop */
                break;
        }

        if(block->flags & MD_BLOCK_CONTAINER) {
            if(block->flags & MD_BLOCK_CONTAINER_CLOSER) {
                MD_LEAVE_BLOCK(block->type, &det);

                if(block->type == MD_BLOCK_UL || block->type == MD_BLOCK_OL || block->type == MD_BLOCK_QUOTE)
                    ctx->n_containers--;
            }

            if(block->flags & MD_BLOCK_CONTAINER_OPENER) {
                MD_ENTER_BLOCK(block->type, &det);

                if(block->type == MD_BLOCK_UL || block->type == MD_BLOCK_OL) {
                    ctx->containers[ctx->n_containers].is_loose = (block->flags & MD_BLOCK_LOOSE_LIST);
                    ctx->n_containers++;
                } else if(block->type == MD_BLOCK_QUOTE) {
                    /* This causes that any text in a block quote, even if
                     * nested inside a tight list item, is wrapped with
                     * <p>...</p>. */
                    ctx->containers[ctx->n_containers].is_loose = TRUE;
                    ctx->n_containers++;
                }
            }
        } else {
            MD_CHECK(md_process_leaf_block(ctx, block));

            if(block->type == MD_BLOCK_CODE || block->type == MD_BLOCK_HTML)
                byte_off += block->n_lines * sizeof(MD_VERBATIMLINE);
            else
                byte_off += block->n_lines * sizeof(MD_LINE);
        }

        byte_off += sizeof(MD_BLOCK);
    }

    ctx->n_block_bytes = 0;

abort:
    return ret;
}