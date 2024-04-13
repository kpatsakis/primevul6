static void dirty_memory_extend(ram_addr_t old_ram_size,
                                ram_addr_t new_ram_size)
{
    ram_addr_t old_num_blocks = DIV_ROUND_UP(old_ram_size,
                                             DIRTY_MEMORY_BLOCK_SIZE);
    ram_addr_t new_num_blocks = DIV_ROUND_UP(new_ram_size,
                                             DIRTY_MEMORY_BLOCK_SIZE);
    int i;

    /* Only need to extend if block count increased */
    if (new_num_blocks <= old_num_blocks) {
        return;
    }

    for (i = 0; i < DIRTY_MEMORY_NUM; i++) {
        DirtyMemoryBlocks *old_blocks;
        DirtyMemoryBlocks *new_blocks;
        int j;

        old_blocks = atomic_rcu_read(&ram_list.dirty_memory[i]);
        new_blocks = g_malloc(sizeof(*new_blocks) +
                              sizeof(new_blocks->blocks[0]) * new_num_blocks);

        if (old_num_blocks) {
            memcpy(new_blocks->blocks, old_blocks->blocks,
                   old_num_blocks * sizeof(old_blocks->blocks[0]));
        }

        for (j = old_num_blocks; j < new_num_blocks; j++) {
            new_blocks->blocks[j] = bitmap_new(DIRTY_MEMORY_BLOCK_SIZE);
        }

        atomic_rcu_set(&ram_list.dirty_memory[i], new_blocks);

        if (old_blocks) {
            g_free_rcu(old_blocks, rcu);
        }
    }
}