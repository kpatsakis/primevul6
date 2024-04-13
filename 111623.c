uc_err uc_mem_protect(struct uc_struct *uc, uint64_t address, size_t size,
                      uint32_t perms)
{
    MemoryRegion *mr;
    uint64_t addr = address;
    size_t count, len;
    bool remove_exec = false;

    UC_INIT(uc);

    if (size == 0) {
        // trivial case, no change
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

    // check for only valid permissions
    if ((perms & ~UC_PROT_ALL) != 0) {
        return UC_ERR_ARG;
    }

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    // check that user's entire requested block is mapped
    if (!check_mem_area(uc, address, size)) {
        return UC_ERR_NOMEM;
    }

    // Now we know entire region is mapped, so change permissions
    // We may need to split regions if this area spans adjacent regions
    addr = address;
    count = 0;
    while (count < size) {
        mr = memory_mapping(uc, addr);
        len = (size_t)MIN(size - count, mr->end - addr);
        if (!split_region(uc, mr, addr, len, false)) {
            return UC_ERR_NOMEM;
        }

        mr = memory_mapping(uc, addr);
        // will this remove EXEC permission?
        if (((mr->perms & UC_PROT_EXEC) != 0) &&
            ((perms & UC_PROT_EXEC) == 0)) {
            remove_exec = true;
        }
        mr->perms = perms;
        uc->readonly_mem(mr, (perms & UC_PROT_WRITE) == 0);

        count += len;
        addr += len;
    }

    // if EXEC permission is removed, then quit TB and continue at the same
    // place
    if (remove_exec) {
        uc->quit_request = true;
        uc_emu_stop(uc);
    }

    return UC_ERR_OK;
}