txt_update_text_state(text_list_entry_t *ppts,
                      const textw_text_enum_t *penum,
                      gs_font *ofont, const gs_matrix *pfmat)
{
    gx_device *const pdev = penum->dev;
    gs_font *font = penum->current_font;
    gs_fixed_point cpt;
    gs_matrix smat, tmat;
    float size;
    int mask = 0;
    int code = gx_path_current_point(penum->path, &cpt);

    if (code < 0)
        return code;

    size = txt_calculate_text_size(penum->pgs, ofont, pfmat, &smat, &tmat, penum->current_font, pdev);
    /* Check for spacing parameters we can handle, and transform them. */

    if (penum->text.operation & TEXT_ADD_TO_ALL_WIDTHS) {
        if (penum->current_font->WMode == 0) {
            gs_point pt;

            code = transform_delta_inverse(&penum->text.delta_all, &smat, &pt);
            if (code < 0 || pt.y != 0)
                mask |= TEXT_ADD_TO_ALL_WIDTHS;
        }
        else
            mask |= TEXT_ADD_TO_ALL_WIDTHS;
    }

    if (penum->text.operation & TEXT_ADD_TO_SPACE_WIDTH) {
        gs_point pt;

        code = transform_delta_inverse(&penum->text.delta_space, &smat, &pt);
        if (code < 0 || pt.y != 0 || penum->text.space.s_char != 32)
            mask |= TEXT_ADD_TO_SPACE_WIDTH;
    }
    /* Store the updated values. */

    tmat.xx /= size;
    tmat.xy /= size;
    tmat.yx /= size;
    tmat.yy /= size;
    tmat.tx += fixed2float(cpt.x);
    tmat.ty += fixed2float(cpt.y);

    ppts->size = size;
    ppts->matrix = tmat;
    ppts->render_mode = penum->pgs->text_rendering_mode;
    ppts->FontName = (char *)gs_malloc(pdev->memory->stable_memory, 1,
        font->font_name.size + 1, "txtwrite alloc font name");
    if (!ppts->FontName)
        return gs_note_error(gs_error_VMerror);
    memcpy(ppts->FontName, font->font_name.chars, font->font_name.size);
    ppts->FontName[font->font_name.size] = 0x00;
    ppts->render_mode = font->WMode;

    if (font->PaintType == 2 && penum->pgs->text_rendering_mode == 0)
    {
        gs_gstate *pgs = penum->pgs;
        gs_font *font = penum->current_font;
        double scaled_width = font->StrokeWidth != 0 ? font->StrokeWidth : 0.001;
        double saved_width = pgs->line_params.half_width;
        /*
         * See stream_to_text in gdevpdfu.c re the computation of
         * the scaling value.
         */
        double scale = 72.0 / pdev->HWResolution[1];

        if (font->FontMatrix.yy != 0)
            scaled_width *= fabs(font->orig_FontMatrix.yy) * size * tmat.yy * scale;
        else
            scaled_width *= fabs(font->orig_FontMatrix.xy) * size * tmat.xy * scale;

        ppts->render_mode = 1;
        ppts->PaintType0Width = scaled_width;

        pgs->line_params.half_width = scaled_width / 2;
        if (code < 0)
            return code;

        pgs->line_params.half_width = saved_width;
    }
    return (code < 0 ? code : mask);
}