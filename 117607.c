md_setup_fenced_code_detail(MD_CTX* ctx, const MD_BLOCK* block, MD_BLOCK_CODE_DETAIL* det,
                            MD_ATTRIBUTE_BUILD* info_build, MD_ATTRIBUTE_BUILD* lang_build)
{
    const MD_VERBATIMLINE* fence_line = (const MD_VERBATIMLINE*)(block + 1);
    OFF beg = fence_line->beg;
    OFF end = fence_line->end;
    OFF lang_end;
    CHAR fence_ch = CH(fence_line->beg);
    int ret = 0;

    /* Skip the fence itself. */
    while(beg < ctx->size  &&  CH(beg) == fence_ch)
        beg++;
    /* Trim initial spaces. */
    while(beg < ctx->size  &&  CH(beg) == _T(' '))
        beg++;

    /* Trim trailing spaces. */
    while(end > beg  &&  CH(end-1) == _T(' '))
        end--;

    /* Build info string attribute. */
    MD_CHECK(md_build_attribute(ctx, STR(beg), end - beg, 0, &det->info, info_build));

    /* Build info string attribute. */
    lang_end = beg;
    while(lang_end < end  &&  !ISWHITESPACE(lang_end))
        lang_end++;
    MD_CHECK(md_build_attribute(ctx, STR(beg), lang_end - beg, 0, &det->lang, lang_build));

    det->fence_char = fence_ch;

abort:
    return ret;
}