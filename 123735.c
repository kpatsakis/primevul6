shift_event(ASS_Renderer *render_priv, EventImages *ei, int shift)
{
    ASS_Image *cur = ei->imgs;
    while (cur) {
        cur->dst_y += shift;
        // clip top and bottom
        if (cur->dst_y < 0) {
            int clip = -cur->dst_y;
            cur->h -= clip;
            cur->bitmap += clip * cur->stride;
            cur->dst_y = 0;
        }
        if (cur->dst_y + cur->h >= render_priv->height) {
            int clip = cur->dst_y + cur->h - render_priv->height;
            cur->h -= clip;
        }
        if (cur->h <= 0) {
            cur->h = 0;
            cur->dst_y = 0;
        }
        cur = cur->next;
    }
    ei->top += shift;
}