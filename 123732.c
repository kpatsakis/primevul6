fix_glyph_scaling(ASS_Renderer *priv, GlyphInfo *glyph)
{
    double ft_size;
    if (priv->settings.hinting == ASS_HINTING_NONE) {
        // arbitrary, not too small to prevent grid fitting rounding effects
        // XXX: this is a rather crude hack
        ft_size = 256.0;
    } else {
        // If hinting is enabled, we want to pass the real font size
        // to freetype. Normalize scale_y to 1.0.
        ft_size = glyph->scale_y * glyph->font_size;
    }
    glyph->scale_x = glyph->scale_x * glyph->font_size / ft_size;
    glyph->scale_y = glyph->scale_y * glyph->font_size / ft_size;
    glyph->font_size = ft_size;
}