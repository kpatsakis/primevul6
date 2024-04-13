static int bsearch_mapped_blocks(const uc_engine *uc, uint64_t address)
{
    int left, right, mid;
    MemoryRegion *mapping;

    left = 0;
    right = uc->mapped_block_count;

    while (left < right) {
        mid = left + (right - left) / 2;

        mapping = uc->mapped_blocks[mid];

        if (mapping->end - 1 < address) {
            left = mid + 1;
        } else if (mapping->addr > address) {
            right = mid;
        } else {
            return mid;
        }
    }

    return left;
}