static bool check_mem_area(uc_engine *uc, uint64_t address, size_t size)
{
    size_t count = 0, len;

    while (count < size) {
        MemoryRegion *mr = memory_mapping(uc, address);
        if (mr) {
            len = (size_t)MIN(size - count, mr->end - address);
            count += len;
            address += len;
        } else { // this address is not mapped in yet
            break;
        }
    }

    return (count == size);
}