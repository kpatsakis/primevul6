static uint32_t pcnet_csr_readw(PCNetState *s, uint32_t rap)
{
    uint32_t val;
    switch (rap) {
    case 0:
        pcnet_update_irq(s);
        val = s->csr[0];
        val |= (val & 0x7800) ? 0x8000 : 0;
        break;
    case 16:
        return pcnet_csr_readw(s,1);
    case 17:
        return pcnet_csr_readw(s,2);
    case 58:
        return pcnet_bcr_readw(s,BCR_SWS);
    case 88:
        val = s->csr[89];
        val <<= 16;
        val |= s->csr[88];
        break;
    default:
        val = s->csr[rap];
    }
#ifdef PCNET_DEBUG_CSR
    printf("pcnet_csr_readw rap=%d val=0x%04x\n", rap, val);
#endif
    return val;
}