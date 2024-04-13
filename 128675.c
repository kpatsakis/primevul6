build_FDArray_sub_font(i_ctx_t *i_ctx_p, ref *op,
                       gs_font_base **ppfont,
                       font_type ftype, gs_memory_type_ptr_t pstype,
                       const build_proc_refs * pbuild,
                       build_font_options_t ignore_options)
{
    gs_font *pfont;
    int code = build_gs_sub_font(i_ctx_p, op, &pfont, ftype, pstype, pbuild,
                                 NULL, op);

    if (code >= 0)
        *ppfont = (gs_font_base *)pfont;
    return code;
}