static inline void ok_jpg_idct_1d_col_16(const int16_t *in, int *out) {
    static const int out_shift = 8;

    int t0, t1, t2;
    int p0, p1, p2, p3, p4, p5, p6, p7;
    int q0, q1, q2, q3, q4, q5, q6, q7;

    for (int x = 0; x < 8; x++) {
        // Quick check to avoid mults
        if (in[1] == 0 && in[2] == 0 && in[3] == 0 && in[4] == 0 &&
            in[5] == 0 && in[6] == 0 && in[7] == 0) {
            t0 = in[0] << (12 - out_shift);
            out[ 0 * 8] = t0;
            out[ 1 * 8] = t0;
            out[ 2 * 8] = t0;
            out[ 3 * 8] = t0;
            out[ 4 * 8] = t0;
            out[ 5 * 8] = t0;
            out[ 6 * 8] = t0;
            out[ 7 * 8] = t0;
            out[ 8 * 8] = t0;
            out[ 9 * 8] = t0;
            out[10 * 8] = t0;
            out[11 * 8] = t0;
            out[12 * 8] = t0;
            out[13 * 8] = t0;
            out[14 * 8] = t0;
            out[15 * 8] = t0;
        } else {
            ok_jpg_idct_1d_16(in[0], in[1], in[2], in[3], in[4], in[5], in[6], in[7]);
            out[ 0 * 8] = (p0 + q0) >> out_shift;
            out[ 1 * 8] = (p1 + q1) >> out_shift;
            out[ 2 * 8] = (p2 + q2) >> out_shift;
            out[ 3 * 8] = (p3 + q3) >> out_shift;
            out[ 4 * 8] = (p4 + q4) >> out_shift;
            out[ 5 * 8] = (p5 + q5) >> out_shift;
            out[ 6 * 8] = (p6 + q6) >> out_shift;
            out[ 7 * 8] = (p7 + q7) >> out_shift;
            out[ 8 * 8] = (p7 - q7) >> out_shift;
            out[ 9 * 8] = (p6 - q6) >> out_shift;
            out[10 * 8] = (p5 - q5) >> out_shift;
            out[11 * 8] = (p4 - q4) >> out_shift;
            out[12 * 8] = (p3 - q3) >> out_shift;
            out[13 * 8] = (p2 - q2) >> out_shift;
            out[14 * 8] = (p1 - q1) >> out_shift;
            out[15 * 8] = (p0 - q0) >> out_shift;
        }

        in += 8;
        out++;
    }
}