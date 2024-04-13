static inline uint8_t ok_jpg_clip_fp_uint8(const int fx) {
    return ((unsigned int)fx) < 0xff0000 ? (uint8_t)(fx >> 16) : (fx < 0 ? 0 : 0xff);
}