static int ehci_state_execute(EHCIQueue *q)
{
    EHCIPacket *p = QTAILQ_FIRST(&q->packets);
    int again = 0;

    assert(p != NULL);
    assert(p->qtdaddr == q->qtdaddr);

    if (ehci_qh_do_overlay(q) != 0) {
        return -1;
    }

    // TODO verify enough time remains in the uframe as in 4.4.1.1
    // TODO write back ptr to async list when done or out of time

    /* 4.10.3, bottom of page 82, go horizontal on transaction counter == 0 */
    if (!q->async && q->transact_ctr == 0) {
        ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);
        again = 1;
        goto out;
    }

    if (q->async) {
        ehci_set_usbsts(q->ehci, USBSTS_REC);
    }

    again = ehci_execute(p, "process");
    if (again == -1) {
        goto out;
    }
    if (p->packet.status == USB_RET_ASYNC) {
        ehci_flush_qh(q);
        trace_usb_ehci_packet_action(p->queue, p, "async");
        p->async = EHCI_ASYNC_INFLIGHT;
        ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);
        if (q->async) {
            again = ehci_fill_queue(p);
        } else {
            again = 1;
        }
        goto out;
    }

    ehci_set_state(q->ehci, q->async, EST_EXECUTING);
    again = 1;

out:
    return again;
}