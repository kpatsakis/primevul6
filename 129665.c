static inline void ehci_update_irq(EHCIState *s)
{
    int level = 0;

    if ((s->usbsts & USBINTR_MASK) & s->usbintr) {
        level = 1;
    }

    trace_usb_ehci_irq(level, s->frindex, s->usbsts, s->usbintr);
    qemu_set_irq(s->irq, level);
}