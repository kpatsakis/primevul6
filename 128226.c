ram_addr_t qemu_ram_addr_from_host(void *ptr)
{
    RAMBlock *block;
    ram_addr_t offset;

    block = qemu_ram_block_from_host(ptr, false, &offset);
    if (!block) {
        return RAM_ADDR_INVALID;
    }

    return block->offset + offset;
}