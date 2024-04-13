RAMBlock *qemu_ram_block_from_host(void *ptr, bool round_offset,
                                   ram_addr_t *offset)
{
    RAMBlock *block;
    uint8_t *host = ptr;

    if (xen_enabled()) {
        ram_addr_t ram_addr;
        rcu_read_lock();
        ram_addr = xen_ram_addr_from_mapcache(ptr);
        block = qemu_get_ram_block(ram_addr);
        if (block) {
            *offset = ram_addr - block->offset;
        }
        rcu_read_unlock();
        return block;
    }

    rcu_read_lock();
    block = atomic_rcu_read(&ram_list.mru_block);
    if (block && block->host && host - block->host < block->max_length) {
        goto found;
    }

    RAMBLOCK_FOREACH(block) {
        /* This case append when the block is not mapped. */
        if (block->host == NULL) {
            continue;
        }
        if (host - block->host < block->max_length) {
            goto found;
        }
    }

    rcu_read_unlock();
    return NULL;

found:
    *offset = (host - block->host);
    if (round_offset) {
        *offset &= TARGET_PAGE_MASK;
    }
    rcu_read_unlock();
    return block;
}