txtwrite_font_orig_matrix(const gs_font *font, gs_glyph cid, gs_matrix *pmat)
{
    int code;

    switch (font->FontType) {
    case ft_composite:                /* subfonts have their own FontMatrix */
    case ft_TrueType:
    case ft_CID_TrueType:
        /* The TrueType FontMatrix is 1 unit per em, which is what we want. */
        gs_make_identity(pmat);
        return 0;
    case ft_encrypted:
    case ft_encrypted2:
    case ft_CID_encrypted:
    case ft_user_defined:
    case ft_PCL_user_defined:
    case ft_GL2_stick_user_defined:
    case ft_GL2_531:
        /*
         * Type 1 fonts are supposed to use a standard FontMatrix of
         * [0.001 0 0 0.001 0 0], with a 1000-unit cell.  However,
         * Windows NT 4.0 creates Type 1 fonts, apparently derived from
         * TrueType fonts, that use a 2048-unit cell and corresponding
         * FontMatrix.  Also, some PS programs perform font scaling by
         * replacing FontMatrix like this :
         *
         *   /f12 /Times-Roman findfont
         *   copyfont          % (remove FID)
         *   dup /FontMatrix [0.012 0 0 0.012 0 0] put
         *   definefont
         *   /f12 1 selectfont
         *
         * Such fonts are their own "base font", but the orig_matrix
         * must still be set to 0.001, not 0.012 .
         *
         * The old code used a heuristic to detect and correct for this here.
         * Unfortunately it doesn't work properly when it meets a font
         * with FontMatrix like this :
         *
         *   /FontMatrix [1 2288 div 0 0 1 2288 div 0 0 ] def
         *
         * (the bug 686970). Also comparefiles\455690.pdf appears to
         * have similar problem. Therefore we added a support to lib/gs_fonts.ps,
         * src/zbfont.c, src/gsfont.c that provides an acces to the original
         * font via a special key OrigFont added to the font dictionary while definefont.
         * Now we work through this access with PS interpreter,
         * but keep the old heuristic for other clients.
         */
        {
            const gs_font *base_font = font;

            while (base_font->base != base_font)
                base_font = base_font->base;
            if (font->FontType == ft_user_defined ||
                font->FontType == ft_PCL_user_defined ||
                font->FontType == ft_GL2_stick_user_defined ||
                font->FontType == ft_GL2_531)
                *pmat = base_font->FontMatrix;
            else if (base_font->orig_FontMatrix.xx != 0 || base_font->orig_FontMatrix.xy != 0 ||
                base_font->orig_FontMatrix.yx != 0 || base_font->orig_FontMatrix.yy != 0)
                *pmat = base_font->orig_FontMatrix;
            else {
                /*  Must not happen with PS interpreter.
                    Provide a hewuristic for other clients.
                */
                if (base_font->FontMatrix.xx == 1.0/2048 &&
                    base_font->FontMatrix.xy == 0 &&
                    base_font->FontMatrix.yx == 0 &&
                    any_abs(base_font->FontMatrix.yy) == 1.0/2048
                    )
                    *pmat = base_font->FontMatrix;
                else
                    gs_make_scaling(0.001, 0.001, pmat);
            }
        }
        if (font->FontType == ft_CID_encrypted && cid != -1) {
            int fidx;

            if (cid < GS_MIN_CID_GLYPH)
                cid = GS_MIN_CID_GLYPH;
            code = ((gs_font_cid0 *)font)->cidata.glyph_data((gs_font_base *)font,
                                cid, NULL, &fidx);
            if (code < 0) {
                code = ((gs_font_cid0 *)font)->cidata.glyph_data((gs_font_base *)font,
                                (gs_glyph)GS_MIN_CID_GLYPH, NULL, &fidx);
            }
            if (code >= 0) {
                gs_matrix_multiply(&(gs_cid0_indexed_font(font, fidx)->FontMatrix),
                                pmat, pmat);
            }
        }
        return 0;
    default:
        return_error(gs_error_rangecheck);
    }
}