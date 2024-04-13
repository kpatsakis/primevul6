uint32_t LEtoUint32(const uint8_t*buffer) {
    uint32_t retval = buffer[3];
    retval <<=8;
    retval |= buffer[2];
    retval <<= 8;
    retval |= buffer[1];
    retval <<= 8;
    retval |= buffer[0];
    return retval;
}