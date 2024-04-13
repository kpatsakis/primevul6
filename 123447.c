sixel_dither_apply_palette(
    sixel_dither_t  /* in */ *dither,
    unsigned char   /* in */ *pixels,
    int             /* in */ width,
    int             /* in */ height)
{
    SIXELSTATUS status = SIXEL_FALSE;
    size_t bufsize;
    unsigned char *dest = NULL;
    int ncolors;
    unsigned char *normalized_pixels = NULL;
    unsigned char *input_pixels;

    if (dither == NULL) {
        sixel_helper_set_additional_message(
            "sixel_dither_apply_palette: dither is null.");
        status = SIXEL_BAD_ARGUMENT;
        goto end;
    }

    sixel_dither_ref(dither);

    bufsize = (size_t)(width * height) * sizeof(unsigned char);
    dest = (unsigned char *)sixel_allocator_malloc(dither->allocator, bufsize);
    if (dest == NULL) {
        sixel_helper_set_additional_message(
            "sixel_dither_new: sixel_allocator_malloc() failed.");
        status = SIXEL_BAD_ALLOCATION;
        goto end;
    }

    /* if quality_mode is full, do not use palette caching */
    if (dither->quality_mode == SIXEL_QUALITY_FULL) {
        dither->optimized = 0;
    }

    if (dither->cachetable == NULL && dither->optimized) {
        if (dither->palette != pal_mono_dark && dither->palette != pal_mono_light) {
            dither->cachetable = (unsigned short *)sixel_allocator_calloc(dither->allocator,
                                                                          (size_t)(1 << 3 * 5),
                                                                          sizeof(unsigned short));
            if (dither->cachetable == NULL) {
                sixel_helper_set_additional_message(
                    "sixel_dither_new: sixel_allocator_calloc() failed.");
                status = SIXEL_BAD_ALLOCATION;
                goto end;
            }
        }
    }

    if (dither->pixelformat != SIXEL_PIXELFORMAT_RGB888) {
        /* normalize pixelformat */
        normalized_pixels
            = (unsigned char *)sixel_allocator_malloc(dither->allocator, (size_t)(width * height * 3));
        if (normalized_pixels == NULL) {
            sixel_helper_set_additional_message(
                "sixel_dither_new: sixel_allocator_malloc() failed.");
            status = SIXEL_BAD_ALLOCATION;
            goto end;
        }
        status = sixel_helper_normalize_pixelformat(normalized_pixels,
                                                    &dither->pixelformat,
                                                    pixels, dither->pixelformat,
                                                    width, height);
        if (SIXEL_FAILED(status)) {
            goto end;
        }
        input_pixels = normalized_pixels;
    } else {
        input_pixels = pixels;
    }

    status = sixel_quant_apply_palette(dest,
                                       input_pixels,
                                       width, height, 3,
                                       dither->palette,
                                       dither->ncolors,
                                       dither->method_for_diffuse,
                                       dither->optimized,
                                       dither->optimize_palette,
                                       dither->complexion,
                                       dither->cachetable,
                                       &ncolors,
                                       dither->allocator);
    if (SIXEL_FAILED(status)) {
        free(dest);
        dest = NULL;
        goto end;
    }

    dither->ncolors = ncolors;

end:
    free(normalized_pixels);
    sixel_dither_unref(dither);
    return dest;
}