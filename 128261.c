static void notdirty_mem_write(void *opaque, hwaddr ram_addr,
                               uint64_t val, unsigned size)
{
    bool locked = false;

    assert(tcg_enabled());
    if (!cpu_physical_memory_get_dirty_flag(ram_addr, DIRTY_MEMORY_CODE)) {
        locked = true;
        tb_lock();
        tb_invalidate_phys_page_fast(ram_addr, size);
    }
    switch (size) {
    case 1:
        stb_p(qemu_map_ram_ptr(NULL, ram_addr), val);
        break;
    case 2:
        stw_p(qemu_map_ram_ptr(NULL, ram_addr), val);
        break;
    case 4:
        stl_p(qemu_map_ram_ptr(NULL, ram_addr), val);
        break;
    default:
        abort();
    }

    if (locked) {
        tb_unlock();
    }

    /* Set both VGA and migration bits for simplicity and to remove
     * the notdirty callback faster.
     */
    cpu_physical_memory_set_dirty_range(ram_addr, size,
                                        DIRTY_CLIENTS_NOCODE);
    /* we remove the notdirty callback only if the code has been
       flushed */
    if (!cpu_physical_memory_is_clean(ram_addr)) {
        tlb_set_dirty(current_cpu, current_cpu->mem_io_vaddr);
    }
}