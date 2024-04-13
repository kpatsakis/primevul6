sixel_dither_set_diffusion_type(
    sixel_dither_t  /* in */ *dither,
    int             /* in */ method_for_diffuse)
{
    if (method_for_diffuse == SIXEL_DIFFUSE_AUTO) {
        if (dither->ncolors > 16) {
            method_for_diffuse = SIXEL_DIFFUSE_FS;
        } else {
            method_for_diffuse = SIXEL_DIFFUSE_ATKINSON;
        }
    }
    dither->method_for_diffuse = method_for_diffuse;
}