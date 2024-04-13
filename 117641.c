md_process_doc(MD_CTX *ctx)
{
    const MD_LINE_ANALYSIS* pivot_line = &md_dummy_blank_line;
    MD_LINE_ANALYSIS line_buf[2];
    MD_LINE_ANALYSIS* line = &line_buf[0];
    OFF off = 0;
    int ret = 0;

    MD_ENTER_BLOCK(MD_BLOCK_DOC, NULL);

    while(off < ctx->size) {
        if(line == pivot_line)
            line = (line == &line_buf[0] ? &line_buf[1] : &line_buf[0]);

        MD_CHECK(md_analyze_line(ctx, off, &off, pivot_line, line));
        MD_CHECK(md_process_line(ctx, &pivot_line, line));
    }

    md_end_current_block(ctx);

    MD_CHECK(md_build_ref_def_hashtable(ctx));

    /* Process all blocks. */
    MD_CHECK(md_leave_child_containers(ctx, 0));
    MD_CHECK(md_process_all_blocks(ctx));

    MD_LEAVE_BLOCK(MD_BLOCK_DOC, NULL);

abort:

#if 0
    /* Output some memory consumption statistics. */
    {
        char buffer[256];
        sprintf(buffer, "Alloced %u bytes for block buffer.",
                    (unsigned)(ctx->alloc_block_bytes));
        MD_LOG(buffer);

        sprintf(buffer, "Alloced %u bytes for containers buffer.",
                    (unsigned)(ctx->alloc_containers * sizeof(MD_CONTAINER)));
        MD_LOG(buffer);

        sprintf(buffer, "Alloced %u bytes for marks buffer.",
                    (unsigned)(ctx->alloc_marks * sizeof(MD_MARK)));
        MD_LOG(buffer);

        sprintf(buffer, "Alloced %u bytes for aux. buffer.",
                    (unsigned)(ctx->alloc_buffer * sizeof(MD_CHAR)));
        MD_LOG(buffer);
    }
#endif

    return ret;
}