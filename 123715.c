get_outline_glyph(ASS_Renderer *priv, GlyphInfo *info)
{
    memset(&info->hash_key, 0, sizeof(info->hash_key));

    OutlineHashKey key;
    OutlineHashValue *val;
    fill_glyph_hash(priv, &key, info);
    if (!ass_cache_get(priv->cache.outline_cache, &key, &val)) {
        if (!val)
            return;
        memset(val, 0, sizeof(*val));

        if (info->drawing) {
            ASS_Drawing *drawing = info->drawing;
            ass_drawing_hash(drawing);
            if(!ass_drawing_parse(drawing, 0)) {
                ass_cache_commit(val, 1);
                ass_cache_dec_ref(val);
                return;
            }
            val->outline = outline_copy(&drawing->outline);
            val->advance.x = drawing->advance.x;
            val->advance.y = drawing->advance.y;
            val->asc = drawing->asc;
            val->desc = drawing->desc;
        } else {
            ass_face_set_size(info->font->faces[info->face_index],
                              info->font_size);
            ass_font_set_transform(info->font, info->scale_x,
                                   info->scale_y, NULL);
            FT_Glyph glyph =
                ass_font_get_glyph(info->font,
                        info->symbol, info->face_index, info->glyph_index,
                        priv->settings.hinting, info->flags);
            if (glyph != NULL) {
                val->outline = outline_convert(&((FT_OutlineGlyph) glyph)->outline);
                if (priv->settings.shaper == ASS_SHAPING_SIMPLE) {
                    val->advance.x = d16_to_d6(glyph->advance.x);
                    val->advance.y = d16_to_d6(glyph->advance.y);
                }
                FT_Done_Glyph(glyph);
                ass_font_get_asc_desc(info->font, info->symbol,
                                      &val->asc, &val->desc);
                val->asc  *= info->scale_y;
                val->desc *= info->scale_y;
            }
        }

        if (!val->outline) {
            ass_cache_commit(val, 1);
            ass_cache_dec_ref(val);
            return;
        }

        outline_get_cbox(val->outline, &val->bbox_scaled);

        if (info->border_style == 3) {
            val->border = calloc(1, sizeof(ASS_Outline));
            if (!val->border) {
                outline_free(val->outline);
                free(val->outline);
                val->outline = NULL;
                ass_cache_commit(val, 1);
                ass_cache_dec_ref(val);
                return;
            }

            FT_Vector advance;
            if (priv->settings.shaper == ASS_SHAPING_SIMPLE || info->drawing)
                advance = val->advance;
            else
                advance = info->advance;

            draw_opaque_box(priv, info, val->asc, val->desc, val->border, advance,
                    double_to_d6(info->border_x * priv->border_scale),
                    double_to_d6(info->border_y * priv->border_scale));

        } else if ((info->border_x > 0 || info->border_y > 0)
                && double_to_d6(info->scale_x) && double_to_d6(info->scale_y)) {

            change_border(priv, info->border_x, info->border_y);
            val->border = outline_copy(val->outline);
            stroke_outline(priv, val->border,
                    double_to_d6(info->border_x * priv->border_scale),
                    double_to_d6(info->border_y * priv->border_scale));
        }

        ass_cache_commit(val, 1);
    }

    if (!val->outline) {
        ass_cache_dec_ref(val);
        return;
    }

    info->hash_key.u.outline.outline = val;
    info->outline = val->outline;
    info->border = val->border;
    info->bbox = val->bbox_scaled;
    if (info->drawing || priv->settings.shaper == ASS_SHAPING_SIMPLE) {
        info->cluster_advance.x = info->advance.x = val->advance.x;
        info->cluster_advance.y = info->advance.y = val->advance.y;
    }
    info->asc = val->asc;
    info->desc = val->desc;
}