static inline uint8_t ok_jpg_clip_uint8(const int x) {
    return ((unsigned int)x) < 0xff ? (uint8_t)x : (x < 0 ? 0 : 0xff);
}