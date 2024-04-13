static void io_mem_init(void)
{
    memory_region_init_io(&io_mem_rom, NULL, &unassigned_mem_ops, NULL, NULL, UINT64_MAX);
    memory_region_init_io(&io_mem_unassigned, NULL, &unassigned_mem_ops, NULL,
                          NULL, UINT64_MAX);

    /* io_mem_notdirty calls tb_invalidate_phys_page_fast,
     * which can be called without the iothread mutex.
     */
    memory_region_init_io(&io_mem_notdirty, NULL, &notdirty_mem_ops, NULL,
                          NULL, UINT64_MAX);
    memory_region_clear_global_locking(&io_mem_notdirty);

    memory_region_init_io(&io_mem_watch, NULL, &watch_mem_ops, NULL,
                          NULL, UINT64_MAX);
}