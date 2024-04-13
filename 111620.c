static uc_err mem_map(uc_engine *uc, uint64_t address, size_t size,
                      uint32_t perms, MemoryRegion *block)
{
    MemoryRegion **regions;
    int pos;

    if (block == NULL) {
        return UC_ERR_NOMEM;
    }

    if ((uc->mapped_block_count & (MEM_BLOCK_INCR - 1)) == 0) { // time to grow
        regions = (MemoryRegion **)g_realloc(
            uc->mapped_blocks,
            sizeof(MemoryRegion *) * (uc->mapped_block_count + MEM_BLOCK_INCR));
        if (regions == NULL) {
            return UC_ERR_NOMEM;
        }
        uc->mapped_blocks = regions;
    }

    pos = bsearch_mapped_blocks(uc, block->addr);

    // shift the array right to give space for the new pointer
    memmove(&uc->mapped_blocks[pos + 1], &uc->mapped_blocks[pos],
            sizeof(MemoryRegion *) * (uc->mapped_block_count - pos));

    uc->mapped_blocks[pos] = block;
    uc->mapped_block_count++;

    return UC_ERR_OK;
}