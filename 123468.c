sixel_dither_get(
    int     /* in */ builtin_dither)
{
    SIXELSTATUS status = SIXEL_FALSE;
    unsigned char *palette;
    int ncolors;
    int keycolor;
    sixel_dither_t *dither = NULL;

    switch (builtin_dither) {
    case SIXEL_BUILTIN_MONO_DARK:
        ncolors = 2;
        palette = (unsigned char *)pal_mono_dark;
        keycolor = 0;
        break;
    case SIXEL_BUILTIN_MONO_LIGHT:
        ncolors = 2;
        palette = (unsigned char *)pal_mono_light;
        keycolor = 0;
        break;
    case SIXEL_BUILTIN_XTERM16:
        ncolors = 16;
        palette = (unsigned char *)pal_xterm256;
        keycolor = (-1);
        break;
    case SIXEL_BUILTIN_XTERM256:
        ncolors = 256;
        palette = (unsigned char *)pal_xterm256;
        keycolor = (-1);
        break;
    case SIXEL_BUILTIN_VT340_MONO:
        ncolors = 16;
        palette = (unsigned char *)pal_vt340_mono;
        keycolor = (-1);
        break;
    case SIXEL_BUILTIN_VT340_COLOR:
        ncolors = 16;
        palette = (unsigned char *)pal_vt340_color;
        keycolor = (-1);
        break;
    case SIXEL_BUILTIN_G1:
        ncolors = 2;
        palette = (unsigned char *)pal_gray_1bit;
        keycolor = (-1);
        break;
    case SIXEL_BUILTIN_G2:
        ncolors = 4;
        palette = (unsigned char *)pal_gray_2bit;
        keycolor = (-1);
        break;
    case SIXEL_BUILTIN_G4:
        ncolors = 16;
        palette = (unsigned char *)pal_gray_4bit;
        keycolor = (-1);
        break;
    case SIXEL_BUILTIN_G8:
        ncolors = 256;
        palette = (unsigned char *)pal_gray_8bit;
        keycolor = (-1);
        break;
    default:
        goto end;
    }

    status = sixel_dither_new(&dither, ncolors, NULL);
    if (SIXEL_FAILED(status)) {
        dither = NULL;
        goto end;
    }

    dither->palette = palette;
    dither->keycolor = keycolor;
    dither->optimized = 1;
    dither->optimize_palette = 0;

end:
    return dither;
}