static void ehci_wakeup_endpoint(USBBus *bus, USBEndpoint *ep,
                                 unsigned int stream)
{
    EHCIState *s = container_of(bus, EHCIState, bus);
    uint32_t portsc = s->portsc[ep->dev->port->index];

    if (portsc & PORTSC_POWNER) {
        return;
    }

    s->periodic_sched_active = PERIODIC_ACTIVE;
    qemu_bh_schedule(s->async_bh);
}