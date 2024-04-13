static inline int put_dwords(EHCIState *ehci, uint32_t addr,
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
        uint32_t tmp = cpu_to_le32(*buf);
        dma_memory_write(ehci->as, addr, &tmp, sizeof(tmp));
    }

    return num;
}