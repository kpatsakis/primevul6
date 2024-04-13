sixel_dither_set_palette(
    sixel_dither_t /* in */ *dither,   /* dither context object */
    unsigned char  /* in */ *palette)
{
    memcpy(dither->palette, palette, (size_t)(dither->ncolors * 3));
}