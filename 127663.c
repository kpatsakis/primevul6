uint32_t pcnet_ioport_readw(void *opaque, uint32_t addr)
{
    PCNetState *s = opaque;
    uint32_t val = -1;
    pcnet_poll_timer(s);
    if (!BCR_DWIO(s)) {
        switch (addr & 0x0f) {
        case 0x00: /* RDP */
            val = pcnet_csr_readw(s, s->rap);
            break;
        case 0x02:
            val = s->rap;
            break;
        case 0x04:
            pcnet_s_reset(s);
            val = 0;
            break;
        case 0x06:
            val = pcnet_bcr_readw(s, s->rap);
            break;
        }
    }
    pcnet_update_irq(s);
#ifdef PCNET_DEBUG_IO
    printf("pcnet_ioport_readw addr=0x%08x val=0x%04x\n", addr, val & 0xffff);
#endif
    return val;
}