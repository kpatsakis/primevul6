unsigned long last_ram_page(void)
{
    RAMBlock *block;
    ram_addr_t last = 0;

    rcu_read_lock();
    RAMBLOCK_FOREACH(block) {
        last = MAX(last, block->offset + block->max_length);
    }
    rcu_read_unlock();
    return last >> TARGET_PAGE_BITS;
}