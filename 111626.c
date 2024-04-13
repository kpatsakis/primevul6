static target_long decode_sleb128(uint8_t **pp)
{
    uint8_t *p = *pp;
    target_long val = 0;
    int byte, shift = 0;

    do {
        byte = *p++;
        val |= (target_ulong)(byte & 0x7f) << shift;
        shift += 7;
    } while (byte & 0x80);
    if (shift < TARGET_LONG_BITS && (byte & 0x40)) {
#ifdef _MSC_VER
        val |= ((target_ulong)0 - 1) << shift;
#else
        val |= -(target_ulong)1 << shift;
#endif
    }

    *pp = p;
    return val;
}