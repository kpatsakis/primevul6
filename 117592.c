md_parse(const MD_CHAR* text, MD_SIZE size, const MD_PARSER* parser, void* userdata)
{
    MD_CTX ctx;
    int i;
    int ret;

    if(parser->abi_version != 0) {
        if(parser->debug_log != NULL)
            parser->debug_log("Unsupported abi_version.", userdata);
        return -1;
    }

    /* Setup context structure. */
    memset(&ctx, 0, sizeof(MD_CTX));
    ctx.text = text;
    ctx.size = size;
    memcpy(&ctx.parser, parser, sizeof(MD_PARSER));
    ctx.userdata = userdata;
    ctx.code_indent_offset = (ctx.parser.flags & MD_FLAG_NOINDENTEDCODEBLOCKS) ? (OFF)(-1) : 4;
    md_build_mark_char_map(&ctx);
    ctx.doc_ends_with_newline = (size > 0  &&  ISNEWLINE_(text[size-1]));

    /* Reset all unresolved opener mark chains. */
    for(i = 0; i < (int) SIZEOF_ARRAY(ctx.mark_chains); i++) {
        ctx.mark_chains[i].head = -1;
        ctx.mark_chains[i].tail = -1;
    }
    ctx.unresolved_link_head = -1;
    ctx.unresolved_link_tail = -1;

    /* All the work. */
    ret = md_process_doc(&ctx);

    /* Clean-up. */
    md_free_ref_defs(&ctx);
    md_free_ref_def_hashtable(&ctx);
    free(ctx.buffer);
    free(ctx.marks);
    free(ctx.block_bytes);
    free(ctx.containers);

    return ret;
}