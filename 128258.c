static MemoryRegionSection *phys_page_find(AddressSpaceDispatch *d, hwaddr addr)
{
    PhysPageEntry lp = d->phys_map, *p;
    Node *nodes = d->map.nodes;
    MemoryRegionSection *sections = d->map.sections;
    hwaddr index = addr >> TARGET_PAGE_BITS;
    int i;

    for (i = P_L2_LEVELS; lp.skip && (i -= lp.skip) >= 0;) {
        if (lp.ptr == PHYS_MAP_NODE_NIL) {
            return &sections[PHYS_SECTION_UNASSIGNED];
        }
        p = nodes[lp.ptr];
        lp = p[(index >> (i * P_L2_BITS)) & (P_L2_SIZE - 1)];
    }

    if (section_covers_addr(&sections[lp.ptr], addr)) {
        return &sections[lp.ptr];
    } else {
        return &sections[PHYS_SECTION_UNASSIGNED];
    }
}