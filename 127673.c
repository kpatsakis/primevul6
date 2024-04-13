uint32_t pcnet_bcr_readw(PCNetState *s, uint32_t rap)
{
    uint32_t val;
    rap &= 127;
    switch (rap) {
    case BCR_LNKST:
    case BCR_LED1:
    case BCR_LED2:
    case BCR_LED3:
        val = s->bcr[rap] & ~0x8000;
        val |= (val & 0x017f & s->lnkst) ? 0x8000 : 0;
        break;
    default:
        val = rap < 32 ? s->bcr[rap] : 0;
        break;
    }
#ifdef PCNET_DEBUG_BCR
    printf("pcnet_bcr_readw rap=%d val=0x%04x\n", rap, val);
#endif
    return val;
}