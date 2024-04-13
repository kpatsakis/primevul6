void update_font(ASS_Renderer *render_priv)
{
    unsigned val;
    ASS_FontDesc desc;

    if (render_priv->state.family[0] == '@') {
        desc.vertical = 1;
        desc.family = strdup(render_priv->state.family + 1);
    } else {
        desc.vertical = 0;
        desc.family = strdup(render_priv->state.family);
    }

    val = render_priv->state.bold;
    // 0 = normal, 1 = bold, >1 = exact weight
    if (val == 1 || val == -1)
        val = 700;               // bold
    else if (val <= 0)
        val = 400;               // normal
    desc.bold = val;

    val = render_priv->state.italic;
    if (val == 1)
        val = 100;              // italic
    else if (val <= 0)
        val = 0;                // normal
    desc.italic = val;

    ass_cache_dec_ref(render_priv->state.font);
    render_priv->state.font =
        ass_font_new(render_priv->cache.font_cache, render_priv->library,
                     render_priv->ftlibrary, render_priv->fontselect,
                     &desc);

    if (render_priv->state.font)
        change_font_size(render_priv, render_priv->state.font_size);
}