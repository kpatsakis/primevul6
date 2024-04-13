static int ehci_state_fetchqtd(EHCIQueue *q)
{
    EHCIqtd qtd;
    EHCIPacket *p;
    int again = 1;

    if (get_dwords(q->ehci, NLPTR_GET(q->qtdaddr), (uint32_t *) &qtd,
                   sizeof(EHCIqtd) >> 2) < 0) {
        return 0;
    }
    ehci_trace_qtd(q, NLPTR_GET(q->qtdaddr), &qtd);

    p = QTAILQ_FIRST(&q->packets);
    if (p != NULL) {
        if (!ehci_verify_qtd(p, &qtd)) {
            ehci_cancel_queue(q);
            if (qtd.token & QTD_TOKEN_ACTIVE) {
                ehci_trace_guest_bug(q->ehci, "guest updated active qTD");
            }
            p = NULL;
        } else {
            p->qtd = qtd;
            ehci_qh_do_overlay(q);
        }
    }

    if (!(qtd.token & QTD_TOKEN_ACTIVE)) {
        ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);
    } else if (p != NULL) {
        switch (p->async) {
        case EHCI_ASYNC_NONE:
        case EHCI_ASYNC_INITIALIZED:
            /* Not yet executed (MULT), or previously nacked (int) packet */
            ehci_set_state(q->ehci, q->async, EST_EXECUTE);
            break;
        case EHCI_ASYNC_INFLIGHT:
            /* Check if the guest has added new tds to the queue */
            again = ehci_fill_queue(QTAILQ_LAST(&q->packets, pkts_head));
            /* Unfinished async handled packet, go horizontal */
            ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);
            break;
        case EHCI_ASYNC_FINISHED:
            /* Complete executing of the packet */
            ehci_set_state(q->ehci, q->async, EST_EXECUTING);
            break;
        }
    } else {
        p = ehci_alloc_packet(q);
        p->qtdaddr = q->qtdaddr;
        p->qtd = qtd;
        ehci_set_state(q->ehci, q->async, EST_EXECUTE);
    }

    return again;
}