md_enter_leave_span_wikilink(MD_CTX* ctx, int enter, const CHAR* target, SZ target_size)
{
    MD_ATTRIBUTE_BUILD target_build = { 0 };
    MD_SPAN_WIKILINK_DETAIL det;
    int ret = 0;

    memset(&det, 0, sizeof(MD_SPAN_WIKILINK_DETAIL));
    MD_CHECK(md_build_attribute(ctx, target, target_size, 0, &det.target, &target_build));

    if (enter)
        MD_ENTER_SPAN(MD_SPAN_WIKILINK, &det);
    else
        MD_LEAVE_SPAN(MD_SPAN_WIKILINK, &det);

abort:
    md_free_attribute(ctx, &target_build);
    return ret;
}