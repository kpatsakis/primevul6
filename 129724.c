static USBDevice *ehci_find_device(EHCIState *ehci, uint8_t addr)
{
    USBDevice *dev;
    USBPort *port;
    int i;

    for (i = 0; i < NB_PORTS; i++) {
        port = &ehci->ports[i];
        if (!(ehci->portsc[i] & PORTSC_PED)) {
            DPRINTF("Port %d not enabled\n", i);
            continue;
        }
        dev = usb_find_device(port, addr);
        if (dev != NULL) {
            return dev;
        }
    }
    return NULL;
}