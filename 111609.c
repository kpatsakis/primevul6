static bool memory_overlap(struct uc_struct *uc, uint64_t begin, size_t size)
{
    unsigned int i;
    uint64_t end = begin + size - 1;

    i = bsearch_mapped_blocks(uc, begin);

    // is this the highest region with no possible overlap?
    if (i >= uc->mapped_block_count)
        return false;

    // end address overlaps this region?
    if (end >= uc->mapped_blocks[i]->addr)
        return true;

    // not found
    return false;
}