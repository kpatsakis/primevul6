build_gs_outline_font(i_ctx_t *i_ctx_p, os_ptr op, gs_font_base ** ppfont,
                      font_type ftype, gs_memory_type_ptr_t pstype,
                      const build_proc_refs * pbuild,
                      build_font_options_t options,
                      build_base_font_proc_t build_base_font)
{
    int painttype;
    float strokewidth;
    gs_font_base *pfont;
    int code = dict_int_param(op, "PaintType", 0, 3, 0, &painttype);

    if (code < 0)
        return code;
    code = dict_float_param(op, "StrokeWidth", 0.0, &strokewidth);
    if (code < 0)
        return code;
    code = build_base_font(i_ctx_p, op, ppfont, ftype, pstype, pbuild,
                           options);
    if (code != 0)
        return code;
    pfont = *ppfont;
    pfont->PaintType = painttype;
    pfont->StrokeWidth = strokewidth;
    return 0;
}