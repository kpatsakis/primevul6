static void ehci_child_detach(USBPort *port, USBDevice *child)
{
    EHCIState *s = port->opaque;
    uint32_t portsc = s->portsc[port->index];

    if (portsc & PORTSC_POWNER) {
        USBPort *companion = s->companion_ports[port->index];
        companion->ops->child_detach(companion, child);
        return;
    }

    ehci_queues_rip_device(s, child, 0);
    ehci_queues_rip_device(s, child, 1);
}