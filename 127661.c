uint32_t pcnet_ioport_readl(void *opaque, uint32_t addr)
{
    PCNetState *s = opaque;
    uint32_t val = -1;
    pcnet_poll_timer(s);
    if (BCR_DWIO(s)) {
        switch (addr & 0x0f) {
        case 0x00: /* RDP */
            val = pcnet_csr_readw(s, s->rap);
            break;
        case 0x04:
            val = s->rap;
            break;
        case 0x08:
            pcnet_s_reset(s);
            val = 0;
            break;
        case 0x0c:
            val = pcnet_bcr_readw(s, s->rap);
            break;
        }
    }
    pcnet_update_irq(s);
#ifdef PCNET_DEBUG_IO
    printf("pcnet_ioport_readl addr=0x%08x val=0x%08x\n", addr, val);
#endif
    return val;
}