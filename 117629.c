md_is_entity(MD_CTX* ctx, OFF beg, OFF max_end, OFF* p_end)
{
    return md_is_entity_str(ctx, ctx->text, beg, max_end, p_end);
}