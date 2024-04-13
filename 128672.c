get_missing_width(gs_font *font, int wmode, const gs_matrix *scale_c,
                    txt_glyph_widths_t *pwidths)
{
    gs_font_info_t finfo;
    int code;

    code = font->procs.font_info((gs_font *)font, NULL,
                                  FONT_INFO_MISSING_WIDTH, &finfo);
    if (code < 0)
        return code;
    if (wmode) {
        gs_distance_transform(0.0, -finfo.MissingWidth, scale_c, &pwidths->real_width.xy);
        pwidths->Width.xy.x = 0;
        pwidths->Width.xy.y = pwidths->real_width.xy.y;
        pwidths->Width.w = pwidths->real_width.w =
                pwidths->Width.xy.y;
        pwidths->Width.v.x = - pwidths->Width.xy.y / 2;
        pwidths->Width.v.y = - pwidths->Width.xy.y;
    } else {
        gs_distance_transform(finfo.MissingWidth, 0.0, scale_c, &pwidths->real_width.xy);
        pwidths->Width.xy.x = pwidths->real_width.xy.x;
        pwidths->Width.xy.y = 0;
        pwidths->Width.w = pwidths->real_width.w =
                pwidths->Width.xy.x;
        pwidths->Width.v.x = pwidths->Width.v.y = 0;
    }
    /*
     * Don't mark the width as known, just in case this is an
     * incrementally defined font.
     */
    return 1;
}