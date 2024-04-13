void cpu_breakpoint_remove_by_ref(CPUState *cpu, CPUBreakpoint *breakpoint)
{
    QTAILQ_REMOVE(&cpu->breakpoints, breakpoint, entry);

    breakpoint_invalidate(cpu, breakpoint->pc);

    g_free(breakpoint);
}