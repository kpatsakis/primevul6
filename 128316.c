static MemTxResult watch_mem_write(void *opaque, hwaddr addr,
                                   uint64_t val, unsigned size,
                                   MemTxAttrs attrs)
{
    MemTxResult res;
    int asidx = cpu_asidx_from_attrs(current_cpu, attrs);
    AddressSpace *as = current_cpu->cpu_ases[asidx].as;

    check_watchpoint(addr & ~TARGET_PAGE_MASK, size, attrs, BP_MEM_WRITE);
    switch (size) {
    case 1:
        address_space_stb(as, addr, val, attrs, &res);
        break;
    case 2:
        address_space_stw(as, addr, val, attrs, &res);
        break;
    case 4:
        address_space_stl(as, addr, val, attrs, &res);
        break;
    default: abort();
    }
    return res;
}