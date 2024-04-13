sixel_dither_ref(
    sixel_dither_t  /* in */ *dither)
{
    /* TODO: be thread safe */
    ++dither->ref;
}