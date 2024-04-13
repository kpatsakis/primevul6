txt_glyph_widths(gs_font *font, int wmode, gs_glyph glyph,
                 gs_font *orig_font, txt_glyph_widths_t *pwidths,
                 const double cdevproc_result[10])
{
    gs_font *ofont = orig_font;
    gs_glyph_info_t info;
    gs_matrix scale_c, scale_o;
    int code, rcode = 0;
    gs_point v;
    int allow_cdevproc_callout = (orig_font->FontType == ft_CID_TrueType
                || orig_font->FontType == ft_CID_encrypted
                ? GLYPH_INFO_CDEVPROC : 0); /* fixme : allow more font types. */

    if (ofont->FontType == ft_composite)
        return_error(gs_error_unregistered); /* Must not happen. */
    code = glyph_orig_matrix((const gs_font *)font, glyph, &scale_c);
    if (code < 0)
        return code;
    code = glyph_orig_matrix(ofont, glyph, &scale_o);
    if (code < 0)
        return code;
    gs_matrix_scale(&scale_c, 1000.0, 1000.0, &scale_c);
    gs_matrix_scale(&scale_o, 1000.0, 1000.0, &scale_o);
    pwidths->Width.v.x = pwidths->Width.v.y = 0;
    pwidths->real_width.v.x = pwidths->real_width.v.y = 0;
    pwidths->replaced_v = false;
    if (glyph == GS_NO_GLYPH)
        return get_missing_width(font, wmode, &scale_c, pwidths);
    code = font->procs.glyph_info((gs_font *)font, glyph, NULL,
                                    GLYPH_INFO_WIDTH0 |
                                    (GLYPH_INFO_WIDTH0 << wmode) |
                                    GLYPH_INFO_OUTLINE_WIDTHS |
                                    (GLYPH_INFO_VVECTOR0 << wmode),
                                    &info);
    /* For CID fonts the PDF spec requires the x-component of v-vector
       to be equal to half glyph width, and AR5 takes it from W, DW.
       So make a compatibe data here.
     */
    if (font->FontType != ft_PCL_user_defined && font->FontType != ft_GL2_stick_user_defined &&
        font->FontType != ft_GL2_531
        && (code == gs_error_undefined || !(info.members & (GLYPH_INFO_WIDTH0 << wmode)))) {
        code = get_missing_width(font, wmode, &scale_c, pwidths);
        if (code < 0)
            v.y = 0;
        else
            v.y = pwidths->Width.v.y;
        if (wmode && (ofont->FontType == ft_CID_encrypted ||
            ofont->FontType == ft_CID_TrueType)) {
            txt_glyph_widths_t widths1;

            if (get_missing_width(font, 0, &scale_c, &widths1) < 0)
                v.x = 0;
            else
                v.x = widths1.Width.w / 2;
        } else
            v.x = pwidths->Width.v.x;
    } else if (code < 0)
        return code;
    else {
        code = store_glyph_width(&pwidths->Width, wmode, &scale_c, &info);
        if (code < 0)
            return code;
        rcode |= code;
        if (info.members  & (GLYPH_INFO_VVECTOR0 << wmode))
            gs_distance_transform(info.v.x, info.v.y, &scale_c, &v);
        else
            v.x = v.y = 0;
        if (wmode && (ofont->FontType == ft_CID_encrypted ||
            ofont->FontType == ft_CID_TrueType)) {
            if (info.members & (GLYPH_INFO_WIDTH0 << wmode)) {
                gs_point xy;

                gs_distance_transform(info.width[0].x, info.width[0].y, &scale_c, &xy);
                v.x = xy.x / 2;
            } else {
                txt_glyph_widths_t widths1;

                if (get_missing_width(font, 0, &scale_c, &widths1) < 0)
                    v.x = 0;
                else
                    v.x = widths1.Width.w / 2;
            }
        }
    }
    pwidths->Width.v = v;
    /* Skip only if not paralel to the axis. */
    if (code > 0 && ofont->FontType != ft_CID_encrypted &&
            ofont->FontType != ft_CID_TrueType)
        pwidths->Width.xy.x = pwidths->Width.xy.y = pwidths->Width.w = 0;
    if (cdevproc_result == NULL) {
        info.members = 0;
        code = ofont->procs.glyph_info(ofont, glyph, NULL,
                                            (GLYPH_INFO_WIDTH0 << wmode) |
                                            (GLYPH_INFO_VVECTOR0 << wmode) |
                                            allow_cdevproc_callout,
                                            &info);
        /* fixme : Move this call before cfont->procs.glyph_info. */
        if (info.members & GLYPH_INFO_CDEVPROC) {
            if (allow_cdevproc_callout)
                return TEXT_PROCESS_CDEVPROC;
        else
            return_error(gs_error_rangecheck);
        }
    } else {
        info.width[0].x = cdevproc_result[0];
        info.width[0].y = cdevproc_result[1];
        info.width[1].x = cdevproc_result[6];
        info.width[1].y = cdevproc_result[7];
        info.v.x = (wmode ? cdevproc_result[8] : 0);
        info.v.y = (wmode ? cdevproc_result[9] : 0);
        info.members = (GLYPH_INFO_WIDTH0 << wmode) |
                       (wmode ? GLYPH_INFO_VVECTOR1 : 0);
        code = 0;
    }
    if (code == gs_error_undefined || !(info.members & (GLYPH_INFO_WIDTH0 << wmode)))
        pwidths->real_width = pwidths->Width;
    else if (code < 0)
        return code;
    else {
        if ((info.members & (GLYPH_INFO_VVECTOR0 | GLYPH_INFO_VVECTOR1)) != 0)
            pwidths->replaced_v = true;
        else
            info.v.x = info.v.y = 0;
        code = store_glyph_width(&pwidths->real_width, wmode, &scale_o, &info);
        if (code < 0)
            return code;
        rcode |= code;
        gs_distance_transform(info.v.x, info.v.y, &scale_o, &pwidths->real_width.v);
    }
    return rcode;
}