md_enter_leave_span_a(MD_CTX* ctx, int enter, MD_SPANTYPE type,
                      const CHAR* dest, SZ dest_size, int prohibit_escapes_in_dest,
                      const CHAR* title, SZ title_size)
{
    MD_ATTRIBUTE_BUILD href_build = { 0 };
    MD_ATTRIBUTE_BUILD title_build = { 0 };
    MD_SPAN_A_DETAIL det;
    int ret = 0;

    /* Note we here rely on fact that MD_SPAN_A_DETAIL and
     * MD_SPAN_IMG_DETAIL are binary-compatible. */
    memset(&det, 0, sizeof(MD_SPAN_A_DETAIL));
    MD_CHECK(md_build_attribute(ctx, dest, dest_size,
                    (prohibit_escapes_in_dest ? MD_BUILD_ATTR_NO_ESCAPES : 0),
                    &det.href, &href_build));
    MD_CHECK(md_build_attribute(ctx, title, title_size, 0, &det.title, &title_build));

    if(enter)
        MD_ENTER_SPAN(type, &det);
    else
        MD_LEAVE_SPAN(type, &det);

abort:
    md_free_attribute(ctx, &href_build);
    md_free_attribute(ctx, &title_build);
    return ret;
}