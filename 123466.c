test1(void)
{
    sixel_dither_t *dither = NULL;
    int nret = EXIT_FAILURE;

#if HAVE_DIAGNOSTIC_DEPRECATED_DECLARATIONS
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    dither = sixel_dither_create(0);
#if HAVE_DIAGNOSTIC_DEPRECATED_DECLARATIONS
#  pragma GCC diagnostic pop
#endif
    if (dither == NULL) {
        goto error;
    }
    sixel_dither_ref(dither);
    sixel_dither_unref(dither);
    nret = EXIT_SUCCESS;

error:
    sixel_dither_unref(dither);
    return nret;
}