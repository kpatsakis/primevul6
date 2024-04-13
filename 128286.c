RAMBlock *qemu_ram_block_by_name(const char *name)
{
    RAMBlock *block;

    RAMBLOCK_FOREACH(block) {
        if (!strcmp(name, block->idstr)) {
            return block;
        }
    }

    return NULL;
}