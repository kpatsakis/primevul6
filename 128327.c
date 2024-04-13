void cpu_watchpoint_remove_by_ref(CPUState *cpu, CPUWatchpoint *watchpoint)
{
    QTAILQ_REMOVE(&cpu->watchpoints, watchpoint, entry);

    tlb_flush_page(cpu, watchpoint->vaddr);

    g_free(watchpoint);
}