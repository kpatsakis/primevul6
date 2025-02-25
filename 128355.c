static inline void cpu_physical_memory_write_rom_internal(AddressSpace *as,
    hwaddr addr, const uint8_t *buf, int len, enum write_rom_type type)
{
    hwaddr l;
    uint8_t *ptr;
    hwaddr addr1;
    MemoryRegion *mr;

    rcu_read_lock();
    while (len > 0) {
        l = len;
        mr = address_space_translate(as, addr, &addr1, &l, true);

        if (!(memory_region_is_ram(mr) ||
              memory_region_is_romd(mr))) {
            l = memory_access_size(mr, l, addr1);
        } else {
            /* ROM/RAM case */
            ptr = qemu_map_ram_ptr(mr->ram_block, addr1);
            switch (type) {
            case WRITE_DATA:
                memcpy(ptr, buf, l);
                invalidate_and_set_dirty(mr, addr1, l);
                break;
            case FLUSH_CACHE:
                flush_icache_range((uintptr_t)ptr, (uintptr_t)ptr + l);
                break;
            }
        }
        len -= l;
        buf += l;
        addr += l;
    }
    rcu_read_unlock();
}