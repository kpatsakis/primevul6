uc_err uc_close(uc_engine *uc)
{
    int i;
    struct list_item *cur;
    struct hook *hook;
    MemoryRegion *mr;

    if (!uc->init_done) {
        free(uc);
        return UC_ERR_OK;
    }

    // Cleanup internally.
    if (uc->release) {
        uc->release(uc->tcg_ctx);
    }
    g_free(uc->tcg_ctx);

    // Cleanup CPU.
    g_free(uc->cpu->cpu_ases);
    g_free(uc->cpu->thread);

    /* cpu */
    free(uc->cpu);

    /* flatviews */
    g_hash_table_destroy(uc->flat_views);

    // During flatviews destruction, we may still access memory regions.
    // So we free them afterwards.
    /* memory */
    mr = &uc->io_mem_unassigned;
    mr->destructor(mr);
    mr = uc->system_io;
    mr->destructor(mr);
    mr = uc->system_memory;
    mr->destructor(mr);
    g_free(uc->system_memory);
    g_free(uc->system_io);

    // Thread relateds.
    if (uc->qemu_thread_data) {
        g_free(uc->qemu_thread_data);
    }

    /* free */
    g_free(uc->init_target_page);

    // Other auxilaries.
    g_free(uc->l1_map);

    if (uc->bounce.buffer) {
        free(uc->bounce.buffer);
    }

    // free hooks and hook lists
    for (i = 0; i < UC_HOOK_MAX; i++) {
        cur = uc->hook[i].head;
        // hook can be in more than one list
        // so we refcount to know when to free
        while (cur) {
            hook = (struct hook *)cur->data;
            if (--hook->refs == 0) {
                free(hook);
            }
            cur = cur->next;
        }
        list_clear(&uc->hook[i]);
    }

    free(uc->mapped_blocks);

    g_tree_destroy(uc->exits);

    // finally, free uc itself.
    memset(uc, 0, sizeof(*uc));
    free(uc);

    return UC_ERR_OK;
}