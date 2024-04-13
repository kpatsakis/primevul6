static void *qemu_ram_ptr_length(RAMBlock *ram_block, ram_addr_t addr,
                                 hwaddr *size)
{
    RAMBlock *block = ram_block;
    if (*size == 0) {
        return NULL;
    }

    if (block == NULL) {
        block = qemu_get_ram_block(addr);
        addr -= block->offset;
    }
    *size = MIN(*size, block->max_length - addr);

    if (xen_enabled() && block->host == NULL) {
        /* We need to check if the requested address is in the RAM
         * because we don't want to map the entire memory in QEMU.
         * In that case just map the requested area.
         */
        if (block->offset == 0) {
            return xen_map_cache(addr, *size, 1, true);
        }

        block->host = xen_map_cache(block->offset, block->max_length, 1, true);
    }

    return ramblock_ptr(block, addr);
}