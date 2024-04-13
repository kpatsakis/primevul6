uc_err uc_mmio_map(uc_engine *uc, uint64_t address, size_t size,
                   uc_cb_mmio_read_t read_cb, void *user_data_read,
                   uc_cb_mmio_write_t write_cb, void *user_data_write)
{
    uc_err res;

    UC_INIT(uc);

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    res = mem_map_check(uc, address, size, UC_PROT_ALL);
    if (res)
        return res;

    // The callbacks do not need to be checked for NULL here, as their presence
    // (or lack thereof) will determine the permissions used.
    return mem_map(uc, address, size, UC_PROT_NONE,
                   uc->memory_map_io(uc, address, size, read_cb, write_cb,
                                     user_data_read, user_data_write));
}