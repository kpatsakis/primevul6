render_glyph(ASS_Renderer *render_priv, Bitmap *bm, int dst_x, int dst_y,
             uint32_t color, uint32_t color2, int brk, ASS_Image **tail,
             unsigned type, CompositeHashValue *source)
{
    // Inverse clipping in use?
    if (render_priv->state.clip_mode)
        return render_glyph_i(render_priv, bm, dst_x, dst_y, color, color2,
                              brk, tail, type, source);

    // brk is relative to dst_x
    // color = color left of brk
    // color2 = color right of brk
    int b_x0, b_y0, b_x1, b_y1; // visible part of the bitmap
    int clip_x0, clip_y0, clip_x1, clip_y1;
    int tmp;
    ASS_Image *img;

    dst_x += bm->left;
    dst_y += bm->top;
    brk -= bm->left;

    // clipping
    clip_x0 = FFMINMAX(render_priv->state.clip_x0, 0, render_priv->width);
    clip_y0 = FFMINMAX(render_priv->state.clip_y0, 0, render_priv->height);
    clip_x1 = FFMINMAX(render_priv->state.clip_x1, 0, render_priv->width);
    clip_y1 = FFMINMAX(render_priv->state.clip_y1, 0, render_priv->height);
    b_x0 = 0;
    b_y0 = 0;
    b_x1 = bm->w;
    b_y1 = bm->h;

    tmp = dst_x - clip_x0;
    if (tmp < 0) {
        b_x0 = -tmp;
        render_priv->state.has_clips = 1;
    }
    tmp = dst_y - clip_y0;
    if (tmp < 0) {
        b_y0 = -tmp;
        render_priv->state.has_clips = 1;
    }
    tmp = clip_x1 - dst_x - bm->w;
    if (tmp < 0) {
        b_x1 = bm->w + tmp;
        render_priv->state.has_clips = 1;
    }
    tmp = clip_y1 - dst_y - bm->h;
    if (tmp < 0) {
        b_y1 = bm->h + tmp;
        render_priv->state.has_clips = 1;
    }

    if ((b_y0 >= b_y1) || (b_x0 >= b_x1))
        return tail;

    if (brk > b_x0) {           // draw left part
        if (brk > b_x1)
            brk = b_x1;
        img = my_draw_bitmap(bm->buffer + bm->stride * b_y0 + b_x0,
                             brk - b_x0, b_y1 - b_y0, bm->stride,
                             dst_x + b_x0, dst_y + b_y0, color, source);
        if (!img) return tail;
        img->type = type;
        *tail = img;
        tail = &img->next;
    }
    if (brk < b_x1) {           // draw right part
        if (brk < b_x0)
            brk = b_x0;
        img = my_draw_bitmap(bm->buffer + bm->stride * b_y0 + brk,
                             b_x1 - brk, b_y1 - b_y0, bm->stride,
                             dst_x + brk, dst_y + b_y0, color2, source);
        if (!img) return tail;
        img->type = type;
        *tail = img;
        tail = &img->next;
    }
    return tail;
}