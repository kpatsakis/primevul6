md_push_container_bytes(MD_CTX* ctx, MD_BLOCKTYPE type, unsigned start,
                        unsigned data, unsigned flags)
{
    MD_BLOCK* block;
    int ret = 0;

    MD_CHECK(md_end_current_block(ctx));

    block = (MD_BLOCK*) md_push_block_bytes(ctx, sizeof(MD_BLOCK));
    if(block == NULL)
        return -1;

    block->type = type;
    block->flags = flags;
    block->data = data;
    block->n_lines = start;

abort:
    return ret;
}