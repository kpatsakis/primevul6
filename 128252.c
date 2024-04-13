void cpu_physical_memory_write_rom(AddressSpace *as, hwaddr addr,
                                   const uint8_t *buf, int len)
{
    cpu_physical_memory_write_rom_internal(as, addr, buf, len, WRITE_DATA);
}