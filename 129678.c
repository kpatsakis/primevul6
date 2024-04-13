static int ehci_cancel_queue(EHCIQueue *q)
{
    EHCIPacket *p;
    int packets = 0;

    p = QTAILQ_FIRST(&q->packets);
    if (p == NULL) {
        goto leave;
    }

    trace_usb_ehci_queue_action(q, "cancel");
    do {
        ehci_free_packet(p);
        packets++;
    } while ((p = QTAILQ_FIRST(&q->packets)) != NULL);

leave:
    ehci_queue_stopped(q);
    return packets;
}