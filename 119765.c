double ensure_font_size(ASS_Renderer *priv, double size)
{
    if (size < 1)
        size = 1;
    else if (size > priv->height * 2)
        size = priv->height * 2;

    return size;
}