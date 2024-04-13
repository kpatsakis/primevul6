static inline bool cpu_watchpoint_address_matches(CPUWatchpoint *wp,
                                                  vaddr addr,
                                                  vaddr len)
{
    /* We know the lengths are non-zero, but a little caution is
     * required to avoid errors in the case where the range ends
     * exactly at the top of the address space and so addr + len
     * wraps round to zero.
     */
    vaddr wpend = wp->vaddr + wp->len - 1;
    vaddr addrend = addr + len - 1;

    return !(addr > wpend || wp->vaddr > addrend);
}