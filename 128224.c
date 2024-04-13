void ram_block_dump(Monitor *mon)
{
    RAMBlock *block;
    char *psize;

    rcu_read_lock();
    monitor_printf(mon, "%24s %8s  %18s %18s %18s\n",
                   "Block Name", "PSize", "Offset", "Used", "Total");
    RAMBLOCK_FOREACH(block) {
        psize = size_to_str(block->page_size);
        monitor_printf(mon, "%24s %8s  0x%016" PRIx64 " 0x%016" PRIx64
                       " 0x%016" PRIx64 "\n", block->idstr, psize,
                       (uint64_t)block->offset,
                       (uint64_t)block->used_length,
                       (uint64_t)block->max_length);
        g_free(psize);
    }
    rcu_read_unlock();
}