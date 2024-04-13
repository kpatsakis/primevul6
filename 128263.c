static void mem_begin(MemoryListener *listener)
{
    AddressSpace *as = container_of(listener, AddressSpace, dispatch_listener);
    AddressSpaceDispatch *d = g_new0(AddressSpaceDispatch, 1);
    uint16_t n;

    n = dummy_section(&d->map, as, &io_mem_unassigned);
    assert(n == PHYS_SECTION_UNASSIGNED);
    n = dummy_section(&d->map, as, &io_mem_notdirty);
    assert(n == PHYS_SECTION_NOTDIRTY);
    n = dummy_section(&d->map, as, &io_mem_rom);
    assert(n == PHYS_SECTION_ROM);
    n = dummy_section(&d->map, as, &io_mem_watch);
    assert(n == PHYS_SECTION_WATCH);

    d->phys_map  = (PhysPageEntry) { .ptr = PHYS_MAP_NODE_NIL, .skip = 1 };
    d->as = as;
    as->next_dispatch = d;
}