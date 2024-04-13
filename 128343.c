DirtyBitmapSnapshot *cpu_physical_memory_snapshot_and_clear_dirty
     (ram_addr_t start, ram_addr_t length, unsigned client)
{
    DirtyMemoryBlocks *blocks;
    unsigned long align = 1UL << (TARGET_PAGE_BITS + BITS_PER_LEVEL);
    ram_addr_t first = QEMU_ALIGN_DOWN(start, align);
    ram_addr_t last  = QEMU_ALIGN_UP(start + length, align);
    DirtyBitmapSnapshot *snap;
    unsigned long page, end, dest;

    snap = g_malloc0(sizeof(*snap) +
                     ((last - first) >> (TARGET_PAGE_BITS + 3)));
    snap->start = first;
    snap->end   = last;

    page = first >> TARGET_PAGE_BITS;
    end  = last  >> TARGET_PAGE_BITS;
    dest = 0;

    rcu_read_lock();

    blocks = atomic_rcu_read(&ram_list.dirty_memory[client]);

    while (page < end) {
        unsigned long idx = page / DIRTY_MEMORY_BLOCK_SIZE;
        unsigned long offset = page % DIRTY_MEMORY_BLOCK_SIZE;
        unsigned long num = MIN(end - page, DIRTY_MEMORY_BLOCK_SIZE - offset);

        assert(QEMU_IS_ALIGNED(offset, (1 << BITS_PER_LEVEL)));
        assert(QEMU_IS_ALIGNED(num,    (1 << BITS_PER_LEVEL)));
        offset >>= BITS_PER_LEVEL;

        bitmap_copy_and_clear_atomic(snap->dirty + dest,
                                     blocks->blocks[idx] + offset,
                                     num);
        page += num;
        dest += num >> BITS_PER_LEVEL;
    }

    rcu_read_unlock();

    if (tcg_enabled()) {
        tlb_reset_dirty_range_all(start, length);
    }

    return snap;
}