IOMMUTLBEntry address_space_get_iotlb_entry(AddressSpace *as, hwaddr addr,
                                            bool is_write)
{
    MemoryRegionSection section;
    hwaddr xlat, plen;

    /* Try to get maximum page mask during translation. */
    plen = (hwaddr)-1;

    /* This can never be MMIO. */
    section = address_space_do_translate(as, addr, &xlat, &plen,
                                         is_write, false);

    /* Illegal translation */
    if (section.mr == &io_mem_unassigned) {
        goto iotlb_fail;
    }

    /* Convert memory region offset into address space offset */
    xlat += section.offset_within_address_space -
        section.offset_within_region;

    if (plen == (hwaddr)-1) {
        /*
         * We use default page size here. Logically it only happens
         * for identity mappings.
         */
        plen = TARGET_PAGE_SIZE;
    }

    /* Convert to address mask */
    plen -= 1;

    return (IOMMUTLBEntry) {
        .target_as = section.address_space,
        .iova = addr & ~plen,
        .translated_addr = xlat & ~plen,
        .addr_mask = plen,
        /* IOTLBs are for DMAs, and DMA only allows on RAMs. */
        .perm = IOMMU_RW,
    };

iotlb_fail:
    return (IOMMUTLBEntry) {0};
}