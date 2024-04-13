static uc_err mem_map_check(uc_engine *uc, uint64_t address, size_t size,
                            uint32_t perms)
{
    if (size == 0) {
        // invalid memory mapping
        return UC_ERR_ARG;
    }

    // address cannot wrapp around
    if (address + size - 1 < address) {
        return UC_ERR_ARG;
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

    // this area overlaps existing mapped regions?
    if (memory_overlap(uc, address, size)) {
        return UC_ERR_MAP;
    }

    return UC_ERR_OK;
}