static inline int get_dwords(EHCIState *ehci, uint32_t addr,
                             uint32_t *buf, int num)
{
    int i;

    if (!ehci->as) {
        ehci_raise_irq(ehci, USBSTS_HSE);
        ehci->usbcmd &= ~USBCMD_RUNSTOP;
        trace_usb_ehci_dma_error();
        return -1;
    }

    for (i = 0; i < num; i++, buf++, addr += sizeof(*buf)) {
        dma_memory_read(ehci->as, addr, buf, sizeof(*buf));
        *buf = le32_to_cpu(*buf);
    }

    return num;
}