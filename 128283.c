void *qemu_map_ram_ptr(RAMBlock *ram_block, ram_addr_t addr)
{
    RAMBlock *block = ram_block;

    if (block == NULL) {
        block = qemu_get_ram_block(addr);
        addr -= block->offset;
    }

    if (xen_enabled() && block->host == NULL) {
        /* We need to check if the requested address is in the RAM
         * because we don't want to map the entire memory in QEMU.
         * In that case just map until the end of the page.
         */
        if (block->offset == 0) {
            return xen_map_cache(addr, 0, 0, false);
        }

        block->host = xen_map_cache(block->offset, block->max_length, 1, false);
    }
    return ramblock_ptr(block, addr);
}