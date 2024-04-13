int cpu_memory_rw_debug(CPUState *cpu, target_ulong addr,
                        uint8_t *buf, int len, int is_write)
{
    int l;
    hwaddr phys_addr;
    target_ulong page;

    cpu_synchronize_state(cpu);
    while (len > 0) {
        int asidx;
        MemTxAttrs attrs;

        page = addr & TARGET_PAGE_MASK;
        phys_addr = cpu_get_phys_page_attrs_debug(cpu, page, &attrs);
        asidx = cpu_asidx_from_attrs(cpu, attrs);
        /* if no physical page mapped, return an error */
        if (phys_addr == -1)
            return -1;
        l = (page + TARGET_PAGE_SIZE) - addr;
        if (l > len)
            l = len;
        phys_addr += (addr & ~TARGET_PAGE_MASK);
        if (is_write) {
            cpu_physical_memory_write_rom(cpu->cpu_ases[asidx].as,
                                          phys_addr, buf, l);
        } else {
            address_space_rw(cpu->cpu_ases[asidx].as, phys_addr,
                             MEMTXATTRS_UNSPECIFIED,
                             buf, l, 0);
        }
        len -= l;
        buf += l;
        addr += l;
    }
    return 0;
}