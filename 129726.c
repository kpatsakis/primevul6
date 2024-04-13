static int ehci_state_fetchitd(EHCIState *ehci, int async)
{
    uint32_t entry;
    EHCIitd itd;

    assert(!async);
    entry = ehci_get_fetch_addr(ehci, async);

    if (get_dwords(ehci, NLPTR_GET(entry), (uint32_t *) &itd,
                   sizeof(EHCIitd) >> 2) < 0) {
        return -1;
    }
    ehci_trace_itd(ehci, entry, &itd);

    if (ehci_process_itd(ehci, &itd, entry) != 0) {
        return -1;
    }

    put_dwords(ehci, NLPTR_GET(entry), (uint32_t *) &itd,
               sizeof(EHCIitd) >> 2);
    ehci_set_fetch_addr(ehci, async, itd.next);
    ehci_set_state(ehci, async, EST_FETCHENTRY);

    return 1;
}