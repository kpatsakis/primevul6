e1000_mmio_write(void *opaque, hwaddr addr, uint64_t val,
                 unsigned size)
{
    E1000State *s = opaque;
    unsigned int index = (addr & 0x1ffff) >> 2;

    if (index < NWRITEOPS && macreg_writeops[index]) {
        if (!(mac_reg_access[index] & MAC_ACCESS_FLAG_NEEDED)
            || (s->compat_flags & (mac_reg_access[index] >> 2))) {
            if (mac_reg_access[index] & MAC_ACCESS_PARTIAL) {
                DBGOUT(GENERAL, "Writing to register at offset: 0x%08x. "
                       "It is not fully implemented.\n", index<<2);
            }
            macreg_writeops[index](s, index, val);
        } else {    /* "flag needed" bit is set, but the flag is not active */
            DBGOUT(MMIO, "MMIO write attempt to disabled reg. addr=0x%08x\n",
                   index<<2);
        }
    } else if (index < NREADOPS && macreg_readops[index]) {
        DBGOUT(MMIO, "e1000_mmio_writel RO %x: 0x%04"PRIx64"\n",
               index<<2, val);
    } else {
        DBGOUT(UNKNOWN, "MMIO unknown write addr=0x%08x,val=0x%08"PRIx64"\n",
               index<<2, val);
    }
}