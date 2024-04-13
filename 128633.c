get_GlyphNames2Unicode(i_ctx_t *i_ctx_p, gs_font *pfont, ref *pdref)
{
    ref *pfontinfo = NULL, *g2u = NULL;
    font_data *pdata;

    if (dict_find_string(pdref, "FontInfo", &pfontinfo) <= 0 ||
            !r_has_type(pfontinfo, t_dictionary) ||
            dict_find_string(pfontinfo, "GlyphNames2Unicode", &g2u) <= 0 ||
            !r_has_type(pfontinfo, t_dictionary))
        return;
    /*
     * Since build_gs_font may resize the dictionary and cause
     * pointers to become invalid, save Glyph2Unicode
     */
    pdata = pfont_data(pfont);
    ref_assign_new(&pdata->GlyphNames2Unicode, g2u);
}