build_gs_FDArray_font(i_ctx_t *i_ctx_p, ref *op,
                      gs_font_base **ppfont,
                      font_type ftype, gs_memory_type_ptr_t pstype,
                      const build_proc_refs * pbuild)
{
    gs_font_base *pfont;
    font_data *pdata;
    int code = build_gs_outline_font(i_ctx_p, op, &pfont, ftype, pstype,
                                     pbuild, bf_options_none,
                                     build_FDArray_sub_font);
    static const double bbox[4] = { 0, 0, 0, 0 };
    gs_uid uid;

    if (code < 0)
        return code;
    pdata = pfont_data(pfont);
    /* Fill in members normally set by build_gs_primitive_font. */
    make_null(&pdata->CharStrings);
    /* Fill in members normally set by build_gs_simple_font. */
    uid_set_invalid(&uid);
    init_gs_simple_font(pfont, bbox, &uid);
    pfont->encoding_index = ENCODING_INDEX_UNKNOWN;
    pfont->nearest_encoding_index = ENCODING_INDEX_UNKNOWN;
    /* Fill in members normally set by build_gs_font. */
    pfont->key_name = pfont->font_name;
    *ppfont = pfont;
    return 0;
}