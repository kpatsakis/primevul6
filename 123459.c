sixel_dither_set_transparent(
    sixel_dither_t /* in */ *dither,      /* dither context object */
    int            /* in */ transparent)  /* transparent color index */
{
    dither->keycolor = transparent;
}