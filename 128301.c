size_t qemu_ram_pagesize_largest(void)
{
    RAMBlock *block;
    size_t largest = 0;

    RAMBLOCK_FOREACH(block) {
        largest = MAX(largest, qemu_ram_pagesize(block));
    }

    return largest;
}