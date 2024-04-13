sixel_dither_set_complexion_score(
    sixel_dither_t /* in */ *dither,  /* dither context object */
    int            /* in */ score)    /* complexion score (>= 1) */
{
    dither->complexion = score;
}