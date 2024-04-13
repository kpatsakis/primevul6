uc_err uc_mem_unmap(struct uc_struct *uc, uint64_t address, size_t size)
{
    MemoryRegion *mr;
    uint64_t addr;
    size_t count, len;

    UC_INIT(uc);

    if (size == 0) {
        // nothing to unmap
        return UC_ERR_OK;
    }

    // address must be aligned to uc->target_page_size
    if ((address & uc->target_page_align) != 0) {
        return UC_ERR_ARG;
    }

    // size must be multiple of uc->target_page_size
    if ((size & uc->target_page_align) != 0) {
        return UC_ERR_ARG;
    }

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    // check that user's entire requested block is mapped
    if (!check_mem_area(uc, address, size)) {
        return UC_ERR_NOMEM;
    }

    // Now we know entire region is mapped, so do the unmap
    // We may need to split regions if this area spans adjacent regions
    addr = address;
    count = 0;
    while (count < size) {
        mr = memory_mapping(uc, addr);
        len = (size_t)MIN(size - count, mr->end - addr);
        if (!split_region(uc, mr, addr, len, true)) {
            return UC_ERR_NOMEM;
        }

        // if we can retrieve the mapping, then no splitting took place
        // so unmap here
        mr = memory_mapping(uc, addr);
        if (mr != NULL) {
            uc->memory_unmap(uc, mr);
        }
        count += len;
        addr += len;
    }

    return UC_ERR_OK;
}