static inline uint16_t readBE16(const uint8_t *data) {
    return (uint16_t)((data[0] << 8) | data[1]);
}