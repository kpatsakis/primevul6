static uint16_t dummy_section(PhysPageMap *map, AddressSpace *as,
                              MemoryRegion *mr)
{
    assert(as);
    MemoryRegionSection section = {
        .address_space = as,
        .mr = mr,
        .offset_within_address_space = 0,
        .offset_within_region = 0,
        .size = int128_2_64(),
    };

    return phys_section_add(map, &section);
}