sixel_dither_initialize(
    sixel_dither_t  /* in */ *dither,
    unsigned char   /* in */ *data,
    int             /* in */ width,
    int             /* in */ height,
    int             /* in */ pixelformat,
    int             /* in */ method_for_largest,
    int             /* in */ method_for_rep,
    int             /* in */ quality_mode)
{
    unsigned char *buf = NULL;
    unsigned char *normalized_pixels = NULL;
    unsigned char *input_pixels;
    SIXELSTATUS status = SIXEL_FALSE;

    if (dither == NULL) {
        sixel_helper_set_additional_message(
            "sixel_dither_new: dither is null.");
        status = SIXEL_BAD_ARGUMENT;
        goto end;
    }

    sixel_dither_ref(dither);

    sixel_dither_set_pixelformat(dither, pixelformat);

    switch (pixelformat) {
    case SIXEL_PIXELFORMAT_RGB888:
        input_pixels = data;
        break;
    default:
        /* normalize pixelformat */
        normalized_pixels
            = (unsigned char *)sixel_allocator_malloc(dither->allocator, (size_t)(width * height * 3));
        if (normalized_pixels == NULL) {
            sixel_helper_set_additional_message(
                "sixel_dither_initialize: sixel_allocator_malloc() failed.");
            status = SIXEL_BAD_ALLOCATION;
            goto end;
        }

        status = sixel_helper_normalize_pixelformat(
            normalized_pixels,
            &pixelformat,
            data,
            pixelformat,
            width,
            height);
        if (SIXEL_FAILED(status)) {
            goto end;
        }
        input_pixels = normalized_pixels;
        break;
    }

    sixel_dither_set_method_for_largest(dither, method_for_largest);
    sixel_dither_set_method_for_rep(dither, method_for_rep);
    sixel_dither_set_quality_mode(dither, quality_mode);

    status = sixel_quant_make_palette(&buf,
                                      input_pixels,
                                      (unsigned int)(width * height * 3),
                                      SIXEL_PIXELFORMAT_RGB888,
                                      (unsigned int)dither->reqcolors,
                                      (unsigned int *)&dither->ncolors,
                                      (unsigned int *)&dither->origcolors,
                                      dither->method_for_largest,
                                      dither->method_for_rep,
                                      dither->quality_mode,
                                      dither->allocator);
    if (SIXEL_FAILED(status)) {
        goto end;
    }
    memcpy(dither->palette, buf, (size_t)(dither->ncolors * 3));

    dither->optimized = 1;
    if (dither->origcolors <= dither->ncolors) {
        dither->method_for_diffuse = SIXEL_DIFFUSE_NONE;
    }

    sixel_quant_free_palette(buf, dither->allocator);
    status = SIXEL_OK;

end:
    free(normalized_pixels);
    sixel_dither_unref(dither);
    return status;
}