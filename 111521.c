uc_err uc_mem_read(uc_engine *uc, uint64_t address, void *_bytes, size_t size)
{
    size_t count = 0, len;
    uint8_t *bytes = _bytes;

    UC_INIT(uc);

    // qemu cpu_physical_memory_rw() size is an int
    if (size > INT_MAX)
        return UC_ERR_ARG;

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    if (!check_mem_area(uc, address, size)) {
        return UC_ERR_READ_UNMAPPED;
    }

    // memory area can overlap adjacent memory blocks
    while (count < size) {
        MemoryRegion *mr = memory_mapping(uc, address);
        if (mr) {
            len = (size_t)MIN(size - count, mr->end - address);
            if (uc->read_mem(&uc->address_space_memory, address, bytes, len) ==
                false) {
                break;
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
        return UC_ERR_READ_UNMAPPED;
    }
}