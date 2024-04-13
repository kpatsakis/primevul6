bool cpu_physical_memory_test_and_clear_dirty(ram_addr_t start,
                                              ram_addr_t length,
                                              unsigned client)
{
    DirtyMemoryBlocks *blocks;
    unsigned long end, page;
    bool dirty = false;

    if (length == 0) {
        return false;
    }

    end = TARGET_PAGE_ALIGN(start + length) >> TARGET_PAGE_BITS;
    page = start >> TARGET_PAGE_BITS;

    rcu_read_lock();

    blocks = atomic_rcu_read(&ram_list.dirty_memory[client]);

    while (page < end) {
        unsigned long idx = page / DIRTY_MEMORY_BLOCK_SIZE;
        unsigned long offset = page % DIRTY_MEMORY_BLOCK_SIZE;
        unsigned long num = MIN(end - page, DIRTY_MEMORY_BLOCK_SIZE - offset);

        dirty |= bitmap_test_and_clear_atomic(blocks->blocks[idx],
                                              offset, num);
        page += num;
    }

    rcu_read_unlock();

    if (dirty && tcg_enabled()) {
        tlb_reset_dirty_range_all(start, length);
    }

    return dirty;
}