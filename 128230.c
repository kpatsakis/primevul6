bool cpu_physical_memory_is_io(hwaddr phys_addr)
{
    MemoryRegion*mr;
    hwaddr l = 1;
    bool res;

    rcu_read_lock();
    mr = address_space_translate(&address_space_memory,
                                 phys_addr, &phys_addr, &l, false);

    res = !(memory_region_is_ram(mr) || memory_region_is_romd(mr));
    rcu_read_unlock();
    return res;
}