static inline void ehci_raise_irq(EHCIState *s, int intr)
{
    if (intr & (USBSTS_PCD | USBSTS_FLR | USBSTS_HSE)) {
        s->usbsts |= intr;
        ehci_update_irq(s);
    } else {
        s->usbsts_pending |= intr;
    }
}