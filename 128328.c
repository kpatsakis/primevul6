static void mem_add(MemoryListener *listener, MemoryRegionSection *section)
{
    AddressSpace *as = container_of(listener, AddressSpace, dispatch_listener);
    AddressSpaceDispatch *d = as->next_dispatch;
    MemoryRegionSection now = *section, remain = *section;
    Int128 page_size = int128_make64(TARGET_PAGE_SIZE);

    if (now.offset_within_address_space & ~TARGET_PAGE_MASK) {
        uint64_t left = TARGET_PAGE_ALIGN(now.offset_within_address_space)
                       - now.offset_within_address_space;

        now.size = int128_min(int128_make64(left), now.size);
        register_subpage(d, &now);
    } else {
        now.size = int128_zero();
    }
    while (int128_ne(remain.size, now.size)) {
        remain.size = int128_sub(remain.size, now.size);
        remain.offset_within_address_space += int128_get64(now.size);
        remain.offset_within_region += int128_get64(now.size);
        now = remain;
        if (int128_lt(remain.size, page_size)) {
            register_subpage(d, &now);
        } else if (remain.offset_within_address_space & ~TARGET_PAGE_MASK) {
            now.size = page_size;
            register_subpage(d, &now);
        } else {
            now.size = int128_and(now.size, int128_neg(page_size));
            register_multipage(d, &now);
        }
    }
}