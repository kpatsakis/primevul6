static inline void ehci_clear_usbsts(EHCIState *s, int mask)
{
    if ((s->usbsts & mask) == 0) {
        return;
    }
    ehci_trace_usbsts(mask, 0);
    s->usbsts &= ~mask;
}