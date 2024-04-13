uc_err uc_mem_map_ptr(uc_engine *uc, uint64_t address, size_t size,
                      uint32_t perms, void *ptr)
{
    uc_err res;

    UC_INIT(uc);

    if (ptr == NULL) {
        return UC_ERR_ARG;
    }

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    res = mem_map_check(uc, address, size, perms);
    if (res) {
        return res;
    }

    return mem_map(uc, address, size, UC_PROT_ALL,
                   uc->memory_map_ptr(uc, address, size, perms, ptr));
}