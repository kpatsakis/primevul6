void qemu_ram_unset_idstr(RAMBlock *block)
{
    /* FIXME: arch_init.c assumes that this is not called throughout
     * migration.  Ignore the problem since hot-unplug during migration
     * does not work anyway.
     */
    if (block) {
        memset(block->idstr, 0, sizeof(block->idstr));
    }
}