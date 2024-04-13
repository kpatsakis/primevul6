static uint8_t *encode_sleb128(uint8_t *p, target_long val)
{
    int more, byte;

    do {
        byte = val & 0x7f;
        val >>= 7;
        more = !((val == 0 && (byte & 0x40) == 0)
                 || (val == -1 && (byte & 0x40) != 0));
        if (more) {
            byte |= 0x80;
        }
        *p++ = byte;
    } while (more);

    return p;
}