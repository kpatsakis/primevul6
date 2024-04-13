static EHCIPacket *ehci_alloc_packet(EHCIQueue *q)
{
    EHCIPacket *p;

    p = g_new0(EHCIPacket, 1);
    p->queue = q;
    usb_packet_init(&p->packet);
    QTAILQ_INSERT_TAIL(&q->packets, p, next);
    trace_usb_ehci_packet_action(p->queue, p, "alloc");
    return p;
}