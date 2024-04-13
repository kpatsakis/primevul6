md_start_new_block(MD_CTX* ctx, const MD_LINE_ANALYSIS* line)
{
    MD_BLOCK* block;

    MD_ASSERT(ctx->current_block == NULL);

    block = (MD_BLOCK*) md_push_block_bytes(ctx, sizeof(MD_BLOCK));
    if(block == NULL)
        return -1;

    switch(line->type) {
        case MD_LINE_HR:
            block->type = MD_BLOCK_HR;
            break;

        case MD_LINE_ATXHEADER:
        case MD_LINE_SETEXTHEADER:
            block->type = MD_BLOCK_H;
            break;

        case MD_LINE_FENCEDCODE:
        case MD_LINE_INDENTEDCODE:
            block->type = MD_BLOCK_CODE;
            break;

        case MD_LINE_TEXT:
            block->type = MD_BLOCK_P;
            break;

        case MD_LINE_HTML:
            block->type = MD_BLOCK_HTML;
            break;

        case MD_LINE_BLANK:
        case MD_LINE_SETEXTUNDERLINE:
        case MD_LINE_TABLEUNDERLINE:
        default:
            MD_UNREACHABLE();
            break;
    }

    block->flags = 0;
    block->data = line->data;
    block->n_lines = 0;

    ctx->current_block = block;
    return 0;
}