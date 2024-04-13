static void ehci_free_packet(EHCIPacket *p)
{
    if (p->async == EHCI_ASYNC_FINISHED &&
            !(p->queue->qh.token & QTD_TOKEN_HALT)) {
        ehci_writeback_async_complete_packet(p);
        return;
    }
    trace_usb_ehci_packet_action(p->queue, p, "free");
    if (p->async == EHCI_ASYNC_INFLIGHT) {
        usb_cancel_packet(&p->packet);
    }
    if (p->async == EHCI_ASYNC_FINISHED &&
            p->packet.status == USB_RET_SUCCESS) {
        fprintf(stderr,
                "EHCI: Dropping completed packet from halted %s ep %02X\n",
                (p->pid == USB_TOKEN_IN) ? "in" : "out",
                get_field(p->queue->qh.epchar, QH_EPCHAR_EP));
    }
    if (p->async != EHCI_ASYNC_NONE) {
        usb_packet_unmap(&p->packet, &p->sgl);
        qemu_sglist_destroy(&p->sgl);
    }
    QTAILQ_REMOVE(&p->queue->packets, p, next);
    usb_packet_cleanup(&p->packet);
    g_free(p);
}