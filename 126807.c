static inline int get_vlc_symbol(GetBitContext *gb, VlcState *const state,
                                 int bits)
{
    int k, i, v, ret;

    i = state->count;
    k = 0;
    while (i < state->error_sum) { // FIXME: optimize
        k++;
        i += i;
    }

    v = get_sr_golomb(gb, k, 12, bits);
    av_dlog(NULL, "v:%d bias:%d error:%d drift:%d count:%d k:%d",
            v, state->bias, state->error_sum, state->drift, state->count, k);

#if 0 // JPEG LS
    if (k == 0 && 2 * state->drift <= -state->count)
        v ^= (-1);
#else
    v ^= ((2 * state->drift + state->count) >> 31);
#endif

    ret = fold(v + state->bias, bits);

    update_vlc_state(state, v);

    return ret;
}