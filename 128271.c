bool set_preferred_target_page_bits(int bits)
{
    /* The target page size is the lowest common denominator for all
     * the CPUs in the system, so we can only make it smaller, never
     * larger. And we can't make it smaller once we've committed to
     * a particular size.
     */
#ifdef TARGET_PAGE_BITS_VARY
    assert(bits >= TARGET_PAGE_BITS_MIN);
    if (target_page_bits == 0 || target_page_bits > bits) {
        if (target_page_bits_decided) {
            return false;
        }
        target_page_bits = bits;
    }
#endif
    return true;
}