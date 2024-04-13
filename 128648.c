txtwrite_process_plain_text(gs_text_enum_t *pte)
{
    /* one byte regular font */
    textw_text_enum_t *const penum = (textw_text_enum_t *)pte;
    gs_font *font = pte->orig_font;
    const gs_glyph *gdata = NULL;
    gs_glyph glyph;
    gs_char ch = 0;
    int i, code;
    uint operation = pte->text.operation;
    txt_glyph_widths_t widths;
    gs_point wanted;	/* user space */
    gs_point dpt = {0,0};

    for (i=0;i<pte->text.size;i++) {
        if (operation & (TEXT_FROM_STRING | TEXT_FROM_BYTES)) {
            ch = pte->text.data.bytes[pte->index++];
        } else if (operation & (TEXT_FROM_CHARS | TEXT_FROM_SINGLE_CHAR)) {
            ch = pte->text.data.chars[pte->index++];
        } else if (operation & (TEXT_FROM_GLYPHS | TEXT_FROM_SINGLE_GLYPH)) {
            if (operation & TEXT_FROM_GLYPHS) {
                gdata = pte->text.data.glyphs + (pte->index++ * sizeof (gs_glyph));
            } else {
                gdata = &pte->text.data.d_glyph;
                pte->index++;
            }
        }
        glyph = (gdata == NULL ? pte->orig_font->procs.encode_char(pte->orig_font, ch, GLYPH_SPACE_NAME)
                           : *gdata);

        code = txt_glyph_widths(font, font->WMode, glyph, (gs_font *)font, &widths, NULL);
        if (code < 0)
            return code;

        penum->cdevproc_callout = false;
        code = txt_update_text_state(penum->text_state, (textw_text_enum_t *)pte, pte->orig_font, &font->FontMatrix);
        if (code < 0)
            return code;

        txt_char_widths_to_uts(pte->orig_font, &widths); /* convert design->text space */
        gs_distance_transform(widths.real_width.xy.x * penum->text_state->size,
                          widths.real_width.xy.y * penum->text_state->size,
                          &penum->text_state->matrix, &wanted);
        pte->returned.total_width.x += wanted.x;
        pte->returned.total_width.y += wanted.y;
        penum->Widths[pte->index - 1] = wanted.x;

        if (pte->text.operation & TEXT_ADD_TO_ALL_WIDTHS) {
            gs_point tpt;

            gs_distance_transform(pte->text.delta_all.x, pte->text.delta_all.y,
                              &ctm_only(pte->pgs), &tpt);
            dpt.x += tpt.x;
            dpt.y += tpt.y;
        }
        if (pte->text.operation & TEXT_ADD_TO_SPACE_WIDTH && ch == pte->text.space.s_char) {
            gs_point tpt;

            gs_distance_transform(pte->text.delta_space.x, pte->text.delta_space.y,
                              &ctm_only(pte->pgs), &tpt);
            dpt.x += tpt.x;
            dpt.y += tpt.y;
        }
        pte->returned.total_width.x += dpt.x;
        pte->returned.total_width.y += dpt.y;

        penum->TextBufferIndex += get_unicode(penum, (gs_font *)pte->orig_font, glyph, ch, &penum->TextBuffer[penum->TextBufferIndex]);
        penum->Widths[pte->index - 1] += dpt.x;
    }
    return 0;
}