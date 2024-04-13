md_end_current_block(MD_CTX* ctx)
{
    int ret = 0;

    if(ctx->current_block == NULL)
        return ret;

    /* Check whether there is a reference definition. (We do this here instead
     * of in md_analyze_line() because reference definition can take multiple
     * lines.) */
    if(ctx->current_block->type == MD_BLOCK_P  ||
       (ctx->current_block->type == MD_BLOCK_H  &&  (ctx->current_block->flags & MD_BLOCK_SETEXT_HEADER)))
    {
        MD_LINE* lines = (MD_LINE*) (ctx->current_block + 1);
        if(CH(lines[0].beg) == _T('[')) {
            MD_CHECK(md_consume_link_reference_definitions(ctx));
            if(ctx->current_block == NULL)
                return ret;
        }
    }

    if(ctx->current_block->type == MD_BLOCK_H  &&  (ctx->current_block->flags & MD_BLOCK_SETEXT_HEADER)) {
        int n_lines = ctx->current_block->n_lines;

        if(n_lines > 1) {
            /* Get rid of the underline. */
            ctx->current_block->n_lines--;
            ctx->n_block_bytes -= sizeof(MD_LINE);
        } else {
            /* Only the underline has left after eating the ref. defs.
             * Keep the line as beginning of a new ordinary paragraph. */
            ctx->current_block->type = MD_BLOCK_P;
            return 0;
        }
    }

    /* Mark we are not building any block anymore. */
    ctx->current_block = NULL;

abort:
    return ret;
}