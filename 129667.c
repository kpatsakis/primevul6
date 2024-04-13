static inline void ehci_set_usbsts(EHCIState *s, int mask)
{
    if ((s->usbsts & mask) == mask) {
        return;
    }
    ehci_trace_usbsts(mask, 1);
    s->usbsts |= mask;
}