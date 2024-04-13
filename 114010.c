static inline void ok_jpg_convert_YCbCr_to_RGB(uint8_t Y, uint8_t Cb, uint8_t Cr,
                                               uint8_t *r, uint8_t *g, uint8_t *b) {
    // From the JFIF spec. Converted to 16:16 fixed point.
    static const int fx1 = 91881; // 1.402
    static const int fx2 = -22553; // 0.34414
    static const int fx3 = -46802; // 0.71414
    static const int fx4 = 116130; // 1.772

    const int fy = (Y << 16) + (1 << 15);
    const int fr = fy + fx1 * (Cr - 128);
    const int fg = fy + fx2 * (Cb - 128) + fx3 * (Cr - 128);
    const int fb = fy + fx4 * (Cb - 128);
    *r = ok_jpg_clip_fp_uint8(fr);
    *g = ok_jpg_clip_fp_uint8(fg);
    *b = ok_jpg_clip_fp_uint8(fb);
}