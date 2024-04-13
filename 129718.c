static void ehci_wakeup(USBPort *port)
{
    EHCIState *s = port->opaque;
    uint32_t *portsc = &s->portsc[port->index];

    if (*portsc & PORTSC_POWNER) {
        USBPort *companion = s->companion_ports[port->index];
        if (companion->ops->wakeup) {
            companion->ops->wakeup(companion);
        }
        return;
    }

    if (*portsc & PORTSC_SUSPEND) {
        trace_usb_ehci_port_wakeup(port->index);
        *portsc |= PORTSC_FPRES;
        ehci_raise_irq(s, USBSTS_PCD);
    }

    qemu_bh_schedule(s->async_bh);
}