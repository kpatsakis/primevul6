static inline void ehci_commit_irq(EHCIState *s)
{
    uint32_t itc;

    if (!s->usbsts_pending) {
        return;
    }
    if (s->usbsts_frindex > s->frindex) {
        return;
    }

    itc = (s->usbcmd >> 16) & 0xff;
    s->usbsts |= s->usbsts_pending;
    s->usbsts_pending = 0;
    s->usbsts_frindex = s->frindex + itc;
    ehci_update_irq(s);
}