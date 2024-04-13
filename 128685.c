build_gs_simple_font(i_ctx_t *i_ctx_p, os_ptr op, gs_font_base ** ppfont,
                     font_type ftype, gs_memory_type_ptr_t pstype,
                     const build_proc_refs * pbuild,
                     build_font_options_t options)
{
    double bbox[4];
    gs_uid uid;
    int code;
    gs_font_base *pfont;
    uint space = ialloc_space(idmemory);

    code = font_bbox_param(imemory, op, bbox);
    if (code < 0)
        return code;
    /*
     * Make sure that we allocate uid
     * in the same VM as the font dictionary
     * (see build_gs_sub_font).
     */
    ialloc_set_space(idmemory, r_space(op));
    code = dict_uid_param(op, &uid, 0, imemory, i_ctx_p);
    ialloc_set_space(idmemory, space);
    if (code < 0)
        return code;
    if ((options & bf_UniqueID_ignored) && uid_is_UniqueID(&uid))
        uid_set_invalid(&uid);
    code = build_gs_font(i_ctx_p, op, (gs_font **) ppfont, ftype, pstype,
                         pbuild, options);
    if (code != 0)		/* invalid or scaled font */
        return code;
    pfont = *ppfont;
    pfont->procs.init_fstack = gs_default_init_fstack;
    pfont->procs.define_font = gs_no_define_font;
    pfont->procs.decode_glyph = gs_font_map_glyph_to_unicode;
    pfont->procs.make_font = zbase_make_font;
    pfont->procs.next_char_glyph = gs_default_next_char_glyph;
    pfont->FAPI = 0;
    pfont->FAPI_font_data = 0;
    init_gs_simple_font(pfont, bbox, &uid);
    lookup_gs_simple_font_encoding(pfont);
    get_GlyphNames2Unicode(i_ctx_p, (gs_font *)pfont, op);
    return 0;
}