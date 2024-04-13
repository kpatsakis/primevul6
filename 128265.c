static MemTxResult watch_mem_read(void *opaque, hwaddr addr, uint64_t *pdata,
                                  unsigned size, MemTxAttrs attrs)
{
    MemTxResult res;
    uint64_t data;
    int asidx = cpu_asidx_from_attrs(current_cpu, attrs);
    AddressSpace *as = current_cpu->cpu_ases[asidx].as;

    check_watchpoint(addr & ~TARGET_PAGE_MASK, size, attrs, BP_MEM_READ);
    switch (size) {
    case 1:
        data = address_space_ldub(as, addr, attrs, &res);
        break;
    case 2:
        data = address_space_lduw(as, addr, attrs, &res);
        break;
    case 4:
        data = address_space_ldl(as, addr, attrs, &res);
        break;
    default: abort();
    }
    *pdata = data;
    return res;
}