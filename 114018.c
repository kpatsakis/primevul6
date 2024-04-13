static inline void ok_jpg_idct_1d_row_16(int h, const int *in, uint8_t *out) {
    static const int out_shift = 19;

    int t0, t1, t2;
    int p0, p1, p2, p3, p4, p5, p6, p7;
    int q0, q1, q2, q3, q4, q5, q6, q7;

    for (int y = 0; y < h; y++) {
        // Quick check to avoid mults
        if (in[1] == 0 && in[2] == 0 && in[3] == 0 && in[4] == 0 &&
            in[5] == 0 && in[6] == 0 && in[7] == 0) {
            const int offset = 1 << (out_shift - 12 - 1);
            t0 = (in[0] + offset) >> (out_shift - 12);
            memset(out, ok_jpg_clip_uint8(t0 + 128), 16);
        } else {
            ok_jpg_idct_1d_16(in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7]);
            out[0]  = ok_jpg_clip_uint8(((p0 + q0) >> out_shift) + 128);
            out[1]  = ok_jpg_clip_uint8(((p1 + q1) >> out_shift) + 128);
            out[2]  = ok_jpg_clip_uint8(((p2 + q2) >> out_shift) + 128);
            out[3]  = ok_jpg_clip_uint8(((p3 + q3) >> out_shift) + 128);
            out[4]  = ok_jpg_clip_uint8(((p4 + q4) >> out_shift) + 128);
            out[5]  = ok_jpg_clip_uint8(((p5 + q5) >> out_shift) + 128);
            out[6]  = ok_jpg_clip_uint8(((p6 + q6) >> out_shift) + 128);
            out[7]  = ok_jpg_clip_uint8(((p7 + q7) >> out_shift) + 128);
            out[8]  = ok_jpg_clip_uint8(((p7 - q7) >> out_shift) + 128);
            out[9]  = ok_jpg_clip_uint8(((p6 - q6) >> out_shift) + 128);
            out[10] = ok_jpg_clip_uint8(((p5 - q5) >> out_shift) + 128);
            out[11] = ok_jpg_clip_uint8(((p4 - q4) >> out_shift) + 128);
            out[12] = ok_jpg_clip_uint8(((p3 - q3) >> out_shift) + 128);
            out[13] = ok_jpg_clip_uint8(((p2 - q2) >> out_shift) + 128);
            out[14] = ok_jpg_clip_uint8(((p1 - q1) >> out_shift) + 128);
            out[15] = ok_jpg_clip_uint8(((p0 - q0) >> out_shift) + 128);
        }
        in += 8;
        out += C_WIDTH;
    }
}