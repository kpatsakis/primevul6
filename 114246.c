e1000_mmio_read(void *opaque, hwaddr addr, unsigned size)
{
    E1000State *s = opaque;
    unsigned int index = (addr & 0x1ffff) >> 2;

    if (index < NREADOPS && macreg_readops[index]) {
        if (!(mac_reg_access[index] & MAC_ACCESS_FLAG_NEEDED)
            || (s->compat_flags & (mac_reg_access[index] >> 2))) {
            if (mac_reg_access[index] & MAC_ACCESS_PARTIAL) {
                DBGOUT(GENERAL, "Reading register at offset: 0x%08x. "
                       "It is not fully implemented.\n", index<<2);
            }
            return macreg_readops[index](s, index);
        } else {    /* "flag needed" bit is set, but the flag is not active */
            DBGOUT(MMIO, "MMIO read attempt of disabled reg. addr=0x%08x\n",
                   index<<2);
        }
    } else {
        DBGOUT(UNKNOWN, "MMIO unknown read addr=0x%08x\n", index<<2);
    }
    return 0;
}