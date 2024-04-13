static inline uint32_t readLE32(const uint8_t *data) {
    return (((uint32_t)data[3] << 24) |
            ((uint32_t)data[2] << 16) |
            ((uint32_t)data[1] << 8) |
            ((uint32_t)data[0] << 0));
}