md_build_mark_char_map(MD_CTX* ctx)
{
    memset(ctx->mark_char_map, 0, sizeof(ctx->mark_char_map));

    ctx->mark_char_map['\\'] = 1;
    ctx->mark_char_map['*'] = 1;
    ctx->mark_char_map['_'] = 1;
    ctx->mark_char_map['`'] = 1;
    ctx->mark_char_map['&'] = 1;
    ctx->mark_char_map[';'] = 1;
    ctx->mark_char_map['<'] = 1;
    ctx->mark_char_map['>'] = 1;
    ctx->mark_char_map['['] = 1;
    ctx->mark_char_map['!'] = 1;
    ctx->mark_char_map[']'] = 1;
    ctx->mark_char_map['\0'] = 1;

    if(ctx->parser.flags & MD_FLAG_STRIKETHROUGH)
        ctx->mark_char_map['~'] = 1;

    if(ctx->parser.flags & MD_FLAG_LATEXMATHSPANS)
        ctx->mark_char_map['$'] = 1;

    if(ctx->parser.flags & MD_FLAG_PERMISSIVEEMAILAUTOLINKS)
        ctx->mark_char_map['@'] = 1;

    if(ctx->parser.flags & MD_FLAG_PERMISSIVEURLAUTOLINKS)
        ctx->mark_char_map[':'] = 1;

    if(ctx->parser.flags & MD_FLAG_PERMISSIVEWWWAUTOLINKS)
        ctx->mark_char_map['.'] = 1;

    if((ctx->parser.flags & MD_FLAG_TABLES) || (ctx->parser.flags & MD_FLAG_WIKILINKS))
        ctx->mark_char_map['|'] = 1;

    if(ctx->parser.flags & MD_FLAG_COLLAPSEWHITESPACE) {
        int i;

        for(i = 0; i < (int) sizeof(ctx->mark_char_map); i++) {
            if(ISWHITESPACE_(i))
                ctx->mark_char_map[i] = 1;
        }
    }
}