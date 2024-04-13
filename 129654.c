static int ehci_state_executing(EHCIQueue *q)
{
    EHCIPacket *p = QTAILQ_FIRST(&q->packets);

    assert(p != NULL);
    assert(p->qtdaddr == q->qtdaddr);

    ehci_execute_complete(q);

    /* 4.10.3 */
    if (!q->async && q->transact_ctr > 0) {
        q->transact_ctr--;
    }

    /* 4.10.5 */
    if (p->packet.status == USB_RET_NAK) {
        ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);
    } else {
        ehci_set_state(q->ehci, q->async, EST_WRITEBACK);
    }

    ehci_flush_qh(q);
    return 1;
}