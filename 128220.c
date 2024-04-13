void qemu_ram_set_idstr(RAMBlock *new_block, const char *name, DeviceState *dev)
{
    RAMBlock *block;

    assert(new_block);
    assert(!new_block->idstr[0]);

    if (dev) {
        char *id = qdev_get_dev_path(dev);
        if (id) {
            snprintf(new_block->idstr, sizeof(new_block->idstr), "%s/", id);
            g_free(id);
        }
    }
    pstrcat(new_block->idstr, sizeof(new_block->idstr), name);

    rcu_read_lock();
    RAMBLOCK_FOREACH(block) {
        if (block != new_block &&
            !strcmp(block->idstr, new_block->idstr)) {
            fprintf(stderr, "RAMBlock \"%s\" already registered, abort!\n",
                    new_block->idstr);
            abort();
        }
    }
    rcu_read_unlock();
}