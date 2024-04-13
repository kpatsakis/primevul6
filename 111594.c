uc_err uc_mem_map(uc_engine *uc, uint64_t address, size_t size, uint32_t perms)
{
    uc_err res;

    UC_INIT(uc);

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    res = mem_map_check(uc, address, size, perms);
    if (res) {
        return res;
    }

    return mem_map(uc, address, size, perms,
                   uc->memory_map(uc, address, size, perms));
}