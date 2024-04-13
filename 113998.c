static inline void ok_jpg_idct_1d_col_8(const int16_t *in, int *out) {
    static const int out_shift = 8;

    int t0, t1, t2;
    int p0, p1, p2, p3;
    int q0, q1, q2, q3;

    for (int x = 0; x < 8; x++) {
        // Quick check to avoid mults
        if (in[1] == 0 && in[2] == 0 && in[3] == 0 && in[4] == 0 &&
            in[5] == 0 && in[6] == 0 && in[7] == 0) {
            t0 = (in[0]) << (12 - out_shift);
            out[0 * 8] = t0;
            out[1 * 8] = t0;
            out[2 * 8] = t0;
            out[3 * 8] = t0;
            out[4 * 8] = t0;
            out[5 * 8] = t0;
            out[6 * 8] = t0;
            out[7 * 8] = t0;
        } else {
            ok_jpg_idct_1d_8(in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7]);
            out[0 * 8] = (p0 + q0) >> out_shift;
            out[1 * 8] = (p1 + q1) >> out_shift;
            out[2 * 8] = (p2 + q2) >> out_shift;
            out[3 * 8] = (p3 + q3) >> out_shift;
            out[4 * 8] = (p3 - q3) >> out_shift;
            out[5 * 8] = (p2 - q2) >> out_shift;
            out[6 * 8] = (p1 - q1) >> out_shift;
            out[7 * 8] = (p0 - q0) >> out_shift;
        }

        in += 8;
        out++;
    }
}