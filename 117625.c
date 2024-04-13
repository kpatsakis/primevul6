md_free_attribute(MD_CTX* ctx, MD_ATTRIBUTE_BUILD* build)
{
    MD_UNUSED(ctx);

    if(build->substr_alloc > 0) {
        free(build->text);
        free(build->substr_types);
        free(build->substr_offsets);
    }
}