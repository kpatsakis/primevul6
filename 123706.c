fill_glyph_hash(ASS_Renderer *priv, OutlineHashKey *outline_key,
                GlyphInfo *info)
{
    if (info->drawing) {
        DrawingHashKey *key = &outline_key->u.drawing;
        outline_key->type = OUTLINE_DRAWING;
        key->scale_x = double_to_d16(info->scale_x);
        key->scale_y = double_to_d16(info->scale_y);
        key->outline.x = double_to_d16(info->border_x);
        key->outline.y = double_to_d16(info->border_y);
        key->border_style = info->border_style;
        // hpacing only matters for opaque box borders (see draw_opaque_box),
        // so for normal borders, maximize cache utility by ignoring it
        key->hspacing =
            info->border_style == 3 ? double_to_d16(info->hspacing) : 0;
        key->hash = info->drawing->hash;
        key->text = info->drawing->text;
        key->pbo = info->drawing->pbo;
        key->scale = info->drawing->scale;
    } else {
        GlyphHashKey *key = &outline_key->u.glyph;
        outline_key->type = OUTLINE_GLYPH;
        key->font = info->font;
        key->size = info->font_size;
        key->face_index = info->face_index;
        key->glyph_index = info->glyph_index;
        key->bold = info->bold;
        key->italic = info->italic;
        key->scale_x = double_to_d16(info->scale_x);
        key->scale_y = double_to_d16(info->scale_y);
        key->outline.x = double_to_d16(info->border_x);
        key->outline.y = double_to_d16(info->border_y);
        key->flags = info->flags;
        key->border_style = info->border_style;
        key->hspacing =
            info->border_style == 3 ? double_to_d16(info->hspacing) : 0;
    }
}