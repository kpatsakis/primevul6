test2(void)
{
    sixel_dither_t *dither = NULL;
    int colors;
    int nret = EXIT_FAILURE;

#if HAVE_DIAGNOSTIC_DEPRECATED_DECLARATIONS
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    dither = sixel_dither_create(INT_MAX);
#if HAVE_DIAGNOSTIC_DEPRECATED_DECLARATIONS
#  pragma GCC diagnostic pop
#endif
    if (dither == NULL) {
        goto error;
    }
    sixel_dither_set_body_only(dither, 1);
    colors = sixel_dither_get_num_of_histogram_colors(dither);
    if (colors != -1) {
        goto error;
    }
    nret = EXIT_SUCCESS;

error:
    sixel_dither_unref(dither);
    return nret;
}