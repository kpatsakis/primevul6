static inline uint32_t readBE32(const uint8_t *data) {
    return (((uint32_t)data[0] << 24) |
            ((uint32_t)data[1] << 16) |
            ((uint32_t)data[2] << 8) |
            ((uint32_t)data[3] << 0));
}