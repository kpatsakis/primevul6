store_glyph_width(txt_glyph_width_t *pwidth, int wmode, const gs_matrix *scale,
                  const gs_glyph_info_t *pinfo)
{
    double w, v;

    gs_distance_transform(pinfo->width[wmode].x, pinfo->width[wmode].y, scale, &pwidth->xy);
    if (wmode)
        w = pwidth->xy.y, v = pwidth->xy.x;
    else
        w = pwidth->xy.x, v = pwidth->xy.y;
    if (v != 0)
        return 1;
    pwidth->w = w;
    gs_distance_transform(pinfo->v.x, pinfo->v.y, scale, &pwidth->v);
    return 0;
}