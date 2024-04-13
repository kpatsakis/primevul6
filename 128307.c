MemoryRegion *iotlb_to_region(CPUState *cpu, hwaddr index, MemTxAttrs attrs)
{
    int asidx = cpu_asidx_from_attrs(cpu, attrs);
    CPUAddressSpace *cpuas = &cpu->cpu_ases[asidx];
    AddressSpaceDispatch *d = atomic_rcu_read(&cpuas->memory_dispatch);
    MemoryRegionSection *sections = d->map.sections;

    return sections[index & ~TARGET_PAGE_MASK].mr;
}