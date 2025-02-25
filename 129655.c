static void ehci_advance_periodic_state(EHCIState *ehci)
{
    uint32_t entry;
    uint32_t list;
    const int async = 0;

    // 4.6

    switch(ehci_get_state(ehci, async)) {
    case EST_INACTIVE:
        if (!(ehci->frindex & 7) && ehci_periodic_enabled(ehci)) {
            ehci_set_state(ehci, async, EST_ACTIVE);
            // No break, fall through to ACTIVE
        } else
            break;

    case EST_ACTIVE:
        if (!(ehci->frindex & 7) && !ehci_periodic_enabled(ehci)) {
            ehci_queues_rip_all(ehci, async);
            ehci_set_state(ehci, async, EST_INACTIVE);
            break;
        }

        list = ehci->periodiclistbase & 0xfffff000;
        /* check that register has been set */
        if (list == 0) {
            break;
        }
        list |= ((ehci->frindex & 0x1ff8) >> 1);

        if (get_dwords(ehci, list, &entry, 1) < 0) {
            break;
        }

        DPRINTF("PERIODIC state adv fr=%d.  [%08X] -> %08X\n",
                ehci->frindex / 8, list, entry);
        ehci_set_fetch_addr(ehci, async,entry);
        ehci_set_state(ehci, async, EST_FETCHENTRY);
        ehci_advance_state(ehci, async);
        ehci_queues_rip_unused(ehci, async);
        break;

    default:
        /* this should only be due to a developer mistake */
        fprintf(stderr, "ehci: Bad periodic state %d. "
                "Resetting to active\n", ehci->pstate);
        g_assert_not_reached();
    }
}