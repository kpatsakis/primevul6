build_gs_sub_font(i_ctx_t *i_ctx_p, const ref *op, gs_font **ppfont,
                  font_type ftype, gs_memory_type_ptr_t pstype,
                  const build_proc_refs * pbuild, const ref *pencoding,
                  ref *fid_op)
{
    gs_matrix mat, omat;
    ref fname;			/* t_string */
    gs_font *pfont;
    font_data *pdata;
    /*
     * Make sure that we allocate the font data
     * in the same VM as the font dictionary.
     */
    uint space = ialloc_space(idmemory);
    int code = sub_font_params(imemory, op, &mat, &omat, &fname);

    if (code < 0)
        return code;
    ialloc_set_space(idmemory, r_space(op));
    pfont = gs_font_alloc(imemory, pstype, &gs_font_procs_default, NULL,
                          "buildfont(font)");
    pdata = ialloc_struct(font_data, &st_font_data,
                          "buildfont(data)");
    if (pfont == 0 || pdata == 0)
        code = gs_note_error(gs_error_VMerror);
    else if (fid_op)
        code = add_FID(i_ctx_p, fid_op, pfont, iimemory);
    if (code < 0) {
        ifree_object(pdata, "buildfont(data)");
        ifree_object(pfont, "buildfont(font)");
        ialloc_set_space(idmemory, space);
        return code;
    }
    refset_null((ref *) pdata, sizeof(font_data) / sizeof(ref));
    ref_assign_new(&pdata->dict, op);
    ref_assign_new(&pdata->BuildChar, &pbuild->BuildChar);
    ref_assign_new(&pdata->BuildGlyph, &pbuild->BuildGlyph);
    if (pencoding)
        ref_assign_new(&pdata->Encoding, pencoding);
    pfont->client_data = pdata;
    pfont->FontType = ftype;
    pfont->FontMatrix = mat;
    pfont->orig_FontMatrix = omat;
    pfont->BitmapWidths = false;
    pfont->ExactSize = fbit_use_bitmaps;
    pfont->InBetweenSize = fbit_use_outlines;
    pfont->TransformedChar = fbit_use_outlines;
    pfont->WMode = 0;
    pfont->procs.encode_char = zfont_encode_char;
    pfont->procs.glyph_name = zfont_glyph_name;
    ialloc_set_space(idmemory, space);
    copy_font_name(&pfont->font_name, &fname);
    *ppfont = pfont;
    return 0;
}