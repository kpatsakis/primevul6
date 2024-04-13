static int usb_ehci_post_load(void *opaque, int version_id)
{
    EHCIState *s = opaque;
    int i;

    for (i = 0; i < NB_PORTS; i++) {
        USBPort *companion = s->companion_ports[i];
        if (companion == NULL) {
            continue;
        }
        if (s->portsc[i] & PORTSC_POWNER) {
            companion->dev = s->ports[i].dev;
        } else {
            companion->dev = NULL;
        }
    }

    return 0;
}