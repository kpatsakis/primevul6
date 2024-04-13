static void ehci_attach(USBPort *port)
{
    EHCIState *s = port->opaque;
    uint32_t *portsc = &s->portsc[port->index];
    const char *owner = (*portsc & PORTSC_POWNER) ? "comp" : "ehci";

    trace_usb_ehci_port_attach(port->index, owner, port->dev->product_desc);

    if (*portsc & PORTSC_POWNER) {
        USBPort *companion = s->companion_ports[port->index];
        companion->dev = port->dev;
        companion->ops->attach(companion);
        return;
    }

    *portsc |= PORTSC_CONNECT;
    *portsc |= PORTSC_CSC;

    ehci_raise_irq(s, USBSTS_PCD);
}