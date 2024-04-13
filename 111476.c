static inline uint32_t get_inc(struct _mdi *mdi, struct _note *nte) {
    int ch = nte->noteid >> 8;
    int32_t note_f;
    uint32_t freq;

    if (__builtin_expect((nte->patch->note != 0), 0)) {
        note_f = nte->patch->note * 100;
    } else {
        note_f = (nte->noteid & 0x7f) * 100;
    }
    note_f += mdi->channel[ch].pitch_adjust;
    if (__builtin_expect((note_f < 0), 0)) {
        note_f = 0;
    } else if (__builtin_expect((note_f > 12700), 0)) {
        note_f = 12700;
    }
    freq = _WM_freq_table[(note_f % 1200)] >> (10 - (note_f / 1200));
    return (((freq / ((_WM_SampleRate * 100) / 1024)) * 1024
             / nte->sample->inc_div));
}