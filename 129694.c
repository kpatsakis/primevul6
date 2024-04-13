static void ehci_async_complete_packet(USBPort *port, USBPacket *packet)
{
    EHCIPacket *p;
    EHCIState *s = port->opaque;
    uint32_t portsc = s->portsc[port->index];

    if (portsc & PORTSC_POWNER) {
        USBPort *companion = s->companion_ports[port->index];
        companion->ops->complete(companion, packet);
        return;
    }

    p = container_of(packet, EHCIPacket, packet);
    assert(p->async == EHCI_ASYNC_INFLIGHT);

    if (packet->status == USB_RET_REMOVE_FROM_QUEUE) {
        trace_usb_ehci_packet_action(p->queue, p, "remove");
        ehci_free_packet(p);
        return;
    }

    trace_usb_ehci_packet_action(p->queue, p, "wakeup");
    p->async = EHCI_ASYNC_FINISHED;

    if (!p->queue->async) {
        s->periodic_sched_active = PERIODIC_ACTIVE;
    }
    qemu_bh_schedule(s->async_bh);
}