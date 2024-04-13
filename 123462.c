sixel_dither_destroy(
    sixel_dither_t  /* in */ *dither)
{
    sixel_allocator_t *allocator;

    if (dither) {
        allocator = dither->allocator;
        sixel_allocator_free(allocator, dither->cachetable);
        dither->cachetable = NULL;
        sixel_allocator_free(allocator, dither);
        sixel_allocator_unref(allocator);
    }
}