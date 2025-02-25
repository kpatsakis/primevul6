static void imdct_and_windowing(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    float *in    = sce->coeffs;
    float *out   = sce->ret;
    float *saved = sce->saved;
    const float *swindow      = ics->use_kb_window[0] ? ff_aac_kbd_short_128 : ff_sine_128;
    const float *lwindow_prev = ics->use_kb_window[1] ? ff_aac_kbd_long_1024 : ff_sine_1024;
    const float *swindow_prev = ics->use_kb_window[1] ? ff_aac_kbd_short_128 : ff_sine_128;
    float *buf  = ac->buf_mdct;
    float *temp = ac->temp;
    int i;

    // imdct
    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        for (i = 0; i < 1024; i += 128)
            ac->mdct_small.imdct_half(&ac->mdct_small, buf + i, in + i);
    } else
        ac->mdct.imdct_half(&ac->mdct, buf, in);

    /* window overlapping
     * NOTE: To simplify the overlapping code, all 'meaningless' short to long
     * and long to short transitions are considered to be short to short
     * transitions. This leaves just two cases (long to long and short to short)
     * with a little special sauce for EIGHT_SHORT_SEQUENCE.
     */
    if ((ics->window_sequence[1] == ONLY_LONG_SEQUENCE || ics->window_sequence[1] == LONG_STOP_SEQUENCE) &&
            (ics->window_sequence[0] == ONLY_LONG_SEQUENCE || ics->window_sequence[0] == LONG_START_SEQUENCE)) {
        ac->fdsp.vector_fmul_window(    out,               saved,            buf,         lwindow_prev, 512);
    } else {
        memcpy(                         out,               saved,            448 * sizeof(float));

        if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
            ac->fdsp.vector_fmul_window(out + 448 + 0*128, saved + 448,      buf + 0*128, swindow_prev, 64);
            ac->fdsp.vector_fmul_window(out + 448 + 1*128, buf + 0*128 + 64, buf + 1*128, swindow,      64);
            ac->fdsp.vector_fmul_window(out + 448 + 2*128, buf + 1*128 + 64, buf + 2*128, swindow,      64);
            ac->fdsp.vector_fmul_window(out + 448 + 3*128, buf + 2*128 + 64, buf + 3*128, swindow,      64);
            ac->fdsp.vector_fmul_window(temp,              buf + 3*128 + 64, buf + 4*128, swindow,      64);
            memcpy(                     out + 448 + 4*128, temp, 64 * sizeof(float));
        } else {
            ac->fdsp.vector_fmul_window(out + 448,         saved + 448,      buf,         swindow_prev, 64);
            memcpy(                     out + 576,         buf + 64,         448 * sizeof(float));
        }
    }

    // buffer update
    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        memcpy(                     saved,       temp + 64,         64 * sizeof(float));
        ac->fdsp.vector_fmul_window(saved + 64,  buf + 4*128 + 64, buf + 5*128, swindow, 64);
        ac->fdsp.vector_fmul_window(saved + 192, buf + 5*128 + 64, buf + 6*128, swindow, 64);
        ac->fdsp.vector_fmul_window(saved + 320, buf + 6*128 + 64, buf + 7*128, swindow, 64);
        memcpy(                     saved + 448, buf + 7*128 + 64,  64 * sizeof(float));
    } else if (ics->window_sequence[0] == LONG_START_SEQUENCE) {
        memcpy(                     saved,       buf + 512,        448 * sizeof(float));
        memcpy(                     saved + 448, buf + 7*128 + 64,  64 * sizeof(float));
    } else { // LONG_STOP or ONLY_LONG
        memcpy(                     saved,       buf + 512,        512 * sizeof(float));
    }
}