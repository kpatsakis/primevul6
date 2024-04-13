md_process_line(MD_CTX* ctx, const MD_LINE_ANALYSIS** p_pivot_line, MD_LINE_ANALYSIS* line)
{
    const MD_LINE_ANALYSIS* pivot_line = *p_pivot_line;
    int ret = 0;

    /* Blank line ends current leaf block. */
    if(line->type == MD_LINE_BLANK) {
        MD_CHECK(md_end_current_block(ctx));
        *p_pivot_line = &md_dummy_blank_line;
        return 0;
    }

    /* Some line types form block on their own. */
    if(line->type == MD_LINE_HR || line->type == MD_LINE_ATXHEADER) {
        MD_CHECK(md_end_current_block(ctx));

        /* Add our single-line block. */
        MD_CHECK(md_start_new_block(ctx, line));
        MD_CHECK(md_add_line_into_current_block(ctx, line));
        MD_CHECK(md_end_current_block(ctx));
        *p_pivot_line = &md_dummy_blank_line;
        return 0;
    }

    /* MD_LINE_SETEXTUNDERLINE changes meaning of the current block and ends it. */
    if(line->type == MD_LINE_SETEXTUNDERLINE) {
        MD_ASSERT(ctx->current_block != NULL);
        ctx->current_block->type = MD_BLOCK_H;
        ctx->current_block->data = line->data;
        ctx->current_block->flags |= MD_BLOCK_SETEXT_HEADER;
        MD_CHECK(md_add_line_into_current_block(ctx, line));
        MD_CHECK(md_end_current_block(ctx));
        if(ctx->current_block == NULL) {
            *p_pivot_line = &md_dummy_blank_line;
        } else {
            /* This happens if we have consumed all the body as link ref. defs.
             * and downgraded the underline into start of a new paragraph block. */
            line->type = MD_LINE_TEXT;
            *p_pivot_line = line;
        }
        return 0;
    }

    /* MD_LINE_TABLEUNDERLINE changes meaning of the current block. */
    if(line->type == MD_LINE_TABLEUNDERLINE) {
        MD_ASSERT(ctx->current_block != NULL);
        MD_ASSERT(ctx->current_block->n_lines == 1);
        ctx->current_block->type = MD_BLOCK_TABLE;
        ctx->current_block->data = line->data;
        MD_ASSERT(pivot_line != &md_dummy_blank_line);
        ((MD_LINE_ANALYSIS*)pivot_line)->type = MD_LINE_TABLE;
        MD_CHECK(md_add_line_into_current_block(ctx, line));
        return 0;
    }

    /* The current block also ends if the line has different type. */
    if(line->type != pivot_line->type)
        MD_CHECK(md_end_current_block(ctx));

    /* The current line may start a new block. */
    if(ctx->current_block == NULL) {
        MD_CHECK(md_start_new_block(ctx, line));
        *p_pivot_line = line;
    }

    /* In all other cases the line is just a continuation of the current block. */
    MD_CHECK(md_add_line_into_current_block(ctx, line));

abort:
    return ret;
}