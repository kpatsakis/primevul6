static inline uint16_t readLE16(const uint8_t *data) {
    return (uint16_t)((data[1] << 8) | data[0]);
}