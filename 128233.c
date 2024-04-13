void cpu_physical_memory_rw(hwaddr addr, uint8_t *buf,
                            int len, int is_write)
{
    address_space_rw(&address_space_memory, addr, MEMTXATTRS_UNSPECIFIED,
                     buf, len, is_write);
}