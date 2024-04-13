txt_char_widths_to_uts(gs_font *font /* may be NULL for non-Type3 */,
                       txt_glyph_widths_t *pwidths)
{
    if (font && (font->FontType == ft_user_defined ||
        font->FontType == ft_PCL_user_defined ||
        font->FontType == ft_GL2_stick_user_defined ||
        font->FontType == ft_GL2_531)) {
        gs_matrix *pmat = &font->orig_FontMatrix;

        pwidths->Width.xy.x *= pmat->xx; /* formula simplified based on wy in glyph space == 0 */
        pwidths->Width.xy.y  = 0.0; /* WMode == 0 for PDF Type 3 fonts */
        gs_distance_transform(pwidths->real_width.xy.x, pwidths->real_width.xy.y, pmat, &pwidths->real_width.xy);
    } else {
        /*
         * For other font types:
         * - PDF design->text space is a simple scaling by 0.001.
         * - The Width.xy.x/y that should be zeroed-out per 5.3.3 "Text Space Details" is already 0.
         */
        pwidths->Width.xy.x /= 1000.0;
        pwidths->Width.xy.y /= 1000.0;
        pwidths->real_width.xy.x /= 1000.0;
        pwidths->real_width.xy.y /= 1000.0;
    }
}