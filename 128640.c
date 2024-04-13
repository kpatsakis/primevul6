textw_text_process(gs_text_enum_t *pte)
{
    textw_text_enum_t *const penum = (textw_text_enum_t *)pte;
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) pte->dev;
    gs_font *font = pte->orig_font;
    gs_font_base *font_base = (gs_font_base *)pte->current_font;
    int code = 0;

    if (pte->text.size == 0)
        return 0;

    if (!penum->TextBuffer) {
        /* We can get up to 4 Unicode points per glyph, and a glyph can be
         * be represented by as little as one byte. So we make a very large
         * temporary buffer to hold the Unicode string as we assemble it. When
         * we copy it to the text fragment we will allocate only as many bytes
         * as are required to hold the actual nukmber of Unicode values we
         * decoded, and this temporary buffer will be discarded.
         */
        penum->TextBuffer = (unsigned short *)gs_malloc(tdev->memory->stable_memory,
            pte->text.size * 4, sizeof(unsigned short), "txtwrite temporary text buffer");
        if (!penum->TextBuffer)
            return gs_note_error(gs_error_VMerror);
        penum->Widths = (float *)gs_malloc(tdev->memory->stable_memory,
            pte->text.size, sizeof(float), "txtwrite temporary widths array");
        if (!penum->Widths)
            return gs_note_error(gs_error_VMerror);
    }
    {
        switch (font->FontType) {
        case ft_CID_encrypted:
        case ft_CID_TrueType:
        case ft_composite:
              code = txtwrite_process_cmap_text(pte);
            break;
        case ft_encrypted:
        case ft_encrypted2:
        case ft_TrueType:
        case ft_user_defined:
        case ft_PCL_user_defined:
        case ft_GL2_stick_user_defined:
        case ft_GL2_531:
            code = txtwrite_process_plain_text(pte);
            break;
        default:
	  return_error(gs_error_rangecheck);
            break;
        }
        if (code == 0) {
            if (font_base->FontBBox.p.x != font_base->FontBBox.q.x ||
                font_base->FontBBox.p.y != font_base->FontBBox.q.y) {
                gs_point p0, p1, p2, p3;
                gs_matrix m;

                m = ctm_only(pte->pgs);
                m.tx = m.ty = fixed2float(0);
                gs_matrix_multiply(&font_base->FontMatrix, &m, &m);
                gs_point_transform(font_base->FontBBox.p.x, font_base->FontBBox.p.y, &m, &p0);
                gs_point_transform(font_base->FontBBox.p.x, font_base->FontBBox.q.y, &m, &p1);
                gs_point_transform(font_base->FontBBox.q.x, font_base->FontBBox.p.y, &m, &p2);
                gs_point_transform(font_base->FontBBox.q.x, font_base->FontBBox.q.y, &m, &p3);
                penum->text_state->FontBBox_bottomleft.x = min(min(p0.x, p1.x), min(p1.x, p2.x));
                penum->text_state->FontBBox_topright.x = max(max(p0.x, p1.x), max(p1.x, p2.x));
                penum->text_state->FontBBox_bottomleft.y = min(min(p0.y, p1.y), min(p1.y, p2.y));
                penum->text_state->FontBBox_topright.y = max(max(p0.y, p1.y), max(p1.y, p2.y));
            }
            code = txt_shift_text_currentpoint(penum, &penum->returned.total_width);
            if (code != 0)
                return code;

            code = txt_add_fragment(tdev, penum);
        }
    }
    return code;
}