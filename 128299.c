MemoryRegion *address_space_translate(AddressSpace *as, hwaddr addr,
                                      hwaddr *xlat, hwaddr *plen,
                                      bool is_write)
{
    MemoryRegion *mr;
    MemoryRegionSection section;

    /* This can be MMIO, so setup MMIO bit. */
    section = address_space_do_translate(as, addr, xlat, plen, is_write, true);
    mr = section.mr;

    if (xen_enabled() && memory_access_is_direct(mr, is_write)) {
        hwaddr page = ((addr & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE) - addr;
        *plen = MIN(page, *plen);
    }

    return mr;
}