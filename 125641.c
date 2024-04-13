static int cbs_av1_write_ns(CodedBitstreamContext *ctx, PutBitContext *pbc,
                            uint32_t n, const char *name,
                            const int *subscripts, uint32_t value)
{
    uint32_t w, m, v, extra_bit;
    int position;

    if (value > n) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "%s out of range: "
               "%"PRIu32", but must be in [0,%"PRIu32"].\n",
               name, value, n);
        return AVERROR_INVALIDDATA;
    }

    if (ctx->trace_enable)
        position = put_bits_count(pbc);

    w = av_log2(n) + 1;
    m = (1 << w) - n;

    if (put_bits_left(pbc) < w)
        return AVERROR(ENOSPC);

    if (value < m) {
        v = value;
        put_bits(pbc, w - 1, v);
    } else {
        v = m + ((value - m) >> 1);
        extra_bit = (value - m) & 1;
        put_bits(pbc, w - 1, v);
        put_bits(pbc, 1, extra_bit);
    }

    if (ctx->trace_enable) {
        char bits[33];
        int i;
        for (i = 0; i < w - 1; i++)
            bits[i] = (v >> i & 1) ? '1' : '0';
        if (value >= m)
            bits[i++] = extra_bit ? '1' : '0';
        bits[i] = 0;

        ff_cbs_trace_syntax_element(ctx, position,
                                    name, subscripts, bits, value);
    }

    return 0;
}