void pcnet_ioport_writew(void *opaque, uint32_t addr, uint32_t val)
{
    PCNetState *s = opaque;
    pcnet_poll_timer(s);
#ifdef PCNET_DEBUG_IO
    printf("pcnet_ioport_writew addr=0x%08x val=0x%04x\n", addr, val);
#endif
    if (!BCR_DWIO(s)) {
        switch (addr & 0x0f) {
        case 0x00: /* RDP */
            pcnet_csr_writew(s, s->rap, val);
            break;
        case 0x02:
            s->rap = val & 0x7f;
            break;
        case 0x06:
            pcnet_bcr_writew(s, s->rap, val);
            break;
        }
    }
    pcnet_update_irq(s);
}