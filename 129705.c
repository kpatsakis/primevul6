static void ehci_execute_complete(EHCIQueue *q)
{
    EHCIPacket *p = QTAILQ_FIRST(&q->packets);
    uint32_t tbytes;

    assert(p != NULL);
    assert(p->qtdaddr == q->qtdaddr);
    assert(p->async == EHCI_ASYNC_INITIALIZED ||
           p->async == EHCI_ASYNC_FINISHED);

    DPRINTF("execute_complete: qhaddr 0x%x, next 0x%x, qtdaddr 0x%x, "
            "status %d, actual_length %d\n",
            q->qhaddr, q->qh.next, q->qtdaddr,
            p->packet.status, p->packet.actual_length);

    switch (p->packet.status) {
    case USB_RET_SUCCESS:
        break;
    case USB_RET_IOERROR:
    case USB_RET_NODEV:
        q->qh.token |= (QTD_TOKEN_HALT | QTD_TOKEN_XACTERR);
        set_field(&q->qh.token, 0, QTD_TOKEN_CERR);
        ehci_raise_irq(q->ehci, USBSTS_ERRINT);
        break;
    case USB_RET_STALL:
        q->qh.token |= QTD_TOKEN_HALT;
        ehci_raise_irq(q->ehci, USBSTS_ERRINT);
        break;
    case USB_RET_NAK:
        set_field(&q->qh.altnext_qtd, 0, QH_ALTNEXT_NAKCNT);
        return; /* We're not done yet with this transaction */
    case USB_RET_BABBLE:
        q->qh.token |= (QTD_TOKEN_HALT | QTD_TOKEN_BABBLE);
        ehci_raise_irq(q->ehci, USBSTS_ERRINT);
        break;
    default:
        /* should not be triggerable */
        fprintf(stderr, "USB invalid response %d\n", p->packet.status);
        g_assert_not_reached();
        break;
    }

    /* TODO check 4.12 for splits */
    tbytes = get_field(q->qh.token, QTD_TOKEN_TBYTES);
    if (tbytes && p->pid == USB_TOKEN_IN) {
        tbytes -= p->packet.actual_length;
        if (tbytes) {
            /* 4.15.1.2 must raise int on a short input packet */
            ehci_raise_irq(q->ehci, USBSTS_INT);
            if (q->async) {
                q->ehci->int_req_by_async = true;
            }
        }
    } else {
        tbytes = 0;
    }
    DPRINTF("updating tbytes to %d\n", tbytes);
    set_field(&q->qh.token, tbytes, QTD_TOKEN_TBYTES);

    ehci_finish_transfer(q, p->packet.actual_length);
    usb_packet_unmap(&p->packet, &p->sgl);
    qemu_sglist_destroy(&p->sgl);
    p->async = EHCI_ASYNC_NONE;

    q->qh.token ^= QTD_TOKEN_DTOGGLE;
    q->qh.token &= ~QTD_TOKEN_ACTIVE;

    if (q->qh.token & QTD_TOKEN_IOC) {
        ehci_raise_irq(q->ehci, USBSTS_INT);
        if (q->async) {
            q->ehci->int_req_by_async = true;
        }
    }
}