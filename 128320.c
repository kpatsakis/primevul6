int cpu_watchpoint_remove(CPUState *cpu, vaddr addr, vaddr len,
                          int flags)
{
    CPUWatchpoint *wp;

    QTAILQ_FOREACH(wp, &cpu->watchpoints, entry) {
        if (addr == wp->vaddr && len == wp->len
                && flags == (wp->flags & ~BP_WATCHPOINT_HIT)) {
            cpu_watchpoint_remove_by_ref(cpu, wp);
            return 0;
        }
    }
    return -ENOENT;
}