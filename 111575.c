uc_err uc_mem_write(uc_engine *uc, uint64_t address, const void *_bytes,
                    size_t size)
{
    size_t count = 0, len;
    const uint8_t *bytes = _bytes;

    UC_INIT(uc);

    // qemu cpu_physical_memory_rw() size is an int
    if (size > INT_MAX)
        return UC_ERR_ARG;

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    if (!check_mem_area(uc, address, size)) {
        return UC_ERR_WRITE_UNMAPPED;
    }

    // memory area can overlap adjacent memory blocks
    while (count < size) {
        MemoryRegion *mr = memory_mapping(uc, address);
        if (mr) {
            uint32_t operms = mr->perms;
            if (!(operms & UC_PROT_WRITE)) { // write protected
                // but this is not the program accessing memory, so temporarily
                // mark writable
                uc->readonly_mem(mr, false);
            }

            len = (size_t)MIN(size - count, mr->end - address);
            if (uc->write_mem(&uc->address_space_memory, address, bytes, len) ==
                false) {
                break;
            }

            if (!(operms & UC_PROT_WRITE)) { // write protected
                // now write protect it again
                uc->readonly_mem(mr, true);
            }

            count += len;
            address += len;
            bytes += len;
        } else { // this address is not mapped in yet
            break;
        }
    }

    if (count == size) {
        return UC_ERR_OK;
    } else {
        return UC_ERR_WRITE_UNMAPPED;
    }
}