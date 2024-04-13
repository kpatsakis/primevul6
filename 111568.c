MemoryRegion *memory_mapping(struct uc_struct *uc, uint64_t address)
{
    unsigned int i;

    if (uc->mapped_block_count == 0) {
        return NULL;
    }

    if (uc->mem_redirect) {
        address = uc->mem_redirect(address);
    }

    // try with the cache index first
    i = uc->mapped_block_cache_index;

    if (i < uc->mapped_block_count && address >= uc->mapped_blocks[i]->addr &&
        address < uc->mapped_blocks[i]->end) {
        return uc->mapped_blocks[i];
    }

    i = bsearch_mapped_blocks(uc, address);

    if (i < uc->mapped_block_count && address >= uc->mapped_blocks[i]->addr &&
        address <= uc->mapped_blocks[i]->end - 1)
        return uc->mapped_blocks[i];

    // not found
    return NULL;
}