md_consume_link_reference_definitions(MD_CTX* ctx)
{
    MD_LINE* lines = (MD_LINE*) (ctx->current_block + 1);
    int n_lines = ctx->current_block->n_lines;
    int n = 0;

    /* Compute how many lines at the start of the block form one or more
     * reference definitions. */
    while(n < n_lines) {
        int n_link_ref_lines;

        n_link_ref_lines = md_is_link_reference_definition(ctx,
                                    lines + n, n_lines - n);
        /* Not a reference definition? */
        if(n_link_ref_lines == 0)
            break;

        /* We fail if it is the ref. def. but it could not be stored due
         * a memory allocation error. */
        if(n_link_ref_lines < 0)
            return -1;

        n += n_link_ref_lines;
    }

    /* If there was at least one reference definition, we need to remove
     * its lines from the block, or perhaps even the whole block. */
    if(n > 0) {
        if(n == n_lines) {
            /* Remove complete block. */
            ctx->n_block_bytes -= n * sizeof(MD_LINE);
            ctx->n_block_bytes -= sizeof(MD_BLOCK);
            ctx->current_block = NULL;
        } else {
            /* Remove just some initial lines from the block. */
            memmove(lines, lines + n, (n_lines - n) * sizeof(MD_LINE));
            ctx->current_block->n_lines -= n;
            ctx->n_block_bytes -= n * sizeof(MD_LINE);
        }
    }

    return 0;
}