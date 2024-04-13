static void invalidate_and_set_dirty(MemoryRegion *mr, hwaddr addr,
                                     hwaddr length)
{
    uint8_t dirty_log_mask = memory_region_get_dirty_log_mask(mr);
    addr += memory_region_get_ram_addr(mr);

    /* No early return if dirty_log_mask is or becomes 0, because
     * cpu_physical_memory_set_dirty_range will still call
     * xen_modified_memory.
     */
    if (dirty_log_mask) {
        dirty_log_mask =
            cpu_physical_memory_range_includes_clean(addr, length, dirty_log_mask);
    }
    if (dirty_log_mask & (1 << DIRTY_MEMORY_CODE)) {
        assert(tcg_enabled());
        tb_lock();
        tb_invalidate_phys_range(addr, addr + length);
        tb_unlock();
        dirty_log_mask &= ~(1 << DIRTY_MEMORY_CODE);
    }
    cpu_physical_memory_set_dirty_range(addr, length, dirty_log_mask);
}