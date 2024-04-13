static void finalize_target_page_bits(void)
{
#ifdef TARGET_PAGE_BITS_VARY
    if (target_page_bits == 0) {
        target_page_bits = TARGET_PAGE_BITS_MIN;
    }
    target_page_bits_decided = true;
#endif
}