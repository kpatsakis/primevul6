static uint64_t ehci_port_read(void *ptr, hwaddr addr,
                               unsigned size)
{
    EHCIState *s = ptr;
    uint32_t val;

    val = s->portsc[addr >> 2];
    trace_usb_ehci_portsc_read(addr + s->portscbase, addr >> 2, val);
    return val;
}