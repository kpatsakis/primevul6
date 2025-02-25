static void decode_sigpass(Jpeg2000T1Context *t1, int width, int height,
                           int bpno, int bandno, int bpass_csty_symbol,
                           int vert_causal_ctx_csty_symbol)
{
    int mask = 3 << (bpno - 1), y0, x, y;

    for (y0 = 0; y0 < height; y0 += 4)
        for (x = 0; x < width; x++)
            for (y = y0; y < height && y < y0 + 4; y++) {
                if ((t1->flags[y+1][x+1] & JPEG2000_T1_SIG_NB)
                && !(t1->flags[y+1][x+1] & (JPEG2000_T1_SIG | JPEG2000_T1_VIS))) {
                    int flags_mask = -1;
                    if (vert_causal_ctx_csty_symbol && y == y0 + 3)
                        flags_mask &= ~(JPEG2000_T1_SIG_S | JPEG2000_T1_SIG_SW | JPEG2000_T1_SIG_SE);
                    if (ff_mqc_decode(&t1->mqc, t1->mqc.cx_states + ff_jpeg2000_getsigctxno(t1->flags[y+1][x+1] & flags_mask, bandno))) {
                        int xorbit, ctxno = ff_jpeg2000_getsgnctxno(t1->flags[y+1][x+1], &xorbit);
                        if (bpass_csty_symbol)
                             t1->data[y][x] = ff_mqc_decode(&t1->mqc, t1->mqc.cx_states + ctxno) ? -mask : mask;
                        else
                             t1->data[y][x] = (ff_mqc_decode(&t1->mqc, t1->mqc.cx_states + ctxno) ^ xorbit) ?
                                               -mask : mask;

                        ff_jpeg2000_set_significance(t1, x, y,
                                                     t1->data[y][x] < 0);
                    }
                    t1->flags[y + 1][x + 1] |= JPEG2000_T1_VIS;
                }
            }
}