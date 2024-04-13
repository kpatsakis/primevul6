void uint32toLE(uint32_t value, uint8_t *retval) {
    retval[0] = uint8_t(value & 0xff);
    retval[1] = uint8_t((value >> 8) & 0xff);
    retval[2] = uint8_t((value >> 16) & 0xff);
    retval[3] = uint8_t((value >> 24) & 0xff);
}