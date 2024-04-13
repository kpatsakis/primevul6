sixel_dither_set_quality_mode(
    sixel_dither_t  /* in */  *dither,
    int             /* in */  quality_mode)
{
    if (quality_mode == SIXEL_QUALITY_AUTO) {
        if (dither->ncolors <= 8) {
            quality_mode = SIXEL_QUALITY_HIGH;
        } else {
            quality_mode = SIXEL_QUALITY_LOW;
        }
    }
    dither->quality_mode = quality_mode;
}