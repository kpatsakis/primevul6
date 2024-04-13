static int ehci_state_horizqh(EHCIQueue *q)
{
    int again = 0;

    if (ehci_get_fetch_addr(q->ehci, q->async) != q->qh.next) {
        ehci_set_fetch_addr(q->ehci, q->async, q->qh.next);
        ehci_set_state(q->ehci, q->async, EST_FETCHENTRY);
        again = 1;
    } else {
        ehci_set_state(q->ehci, q->async, EST_ACTIVE);
    }

    return again;
}