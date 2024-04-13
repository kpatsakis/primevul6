sixel_dither_create(
    int     /* in */ ncolors)
{
    SIXELSTATUS status = SIXEL_FALSE;
    sixel_dither_t *dither = NULL;

    status = sixel_dither_new(&dither, ncolors, NULL);
    if (SIXEL_FAILED(status)) {
        goto end;
    }

end:
    return dither;
}