static int cbs_av1_read_ns(CodedBitstreamContext *ctx, GetBitContext *gbc,
                           uint32_t n, const char *name,
                           const int *subscripts, uint32_t *write_to)
{
    uint32_t w, m, v, extra_bit, value;
    int position;

    av_assert0(n > 0);

    if (ctx->trace_enable)
        position = get_bits_count(gbc);

    w = av_log2(n) + 1;
    m = (1 << w) - n;

    if (get_bits_left(gbc) < w) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "Invalid non-symmetric value at "
               "%s: bitstream ended.\n", name);
        return AVERROR_INVALIDDATA;
    }

    if (w - 1 > 0)
        v = get_bits(gbc, w - 1);
    else
        v = 0;

    if (v < m) {
        value = v;
    } else {
        extra_bit = get_bits1(gbc);
        value = (v << 1) - m + extra_bit;
    }

    if (ctx->trace_enable) {
        char bits[33];
        int i;
        for (i = 0; i < w - 1; i++)
            bits[i] = (v >> i & 1) ? '1' : '0';
        if (v >= m)
            bits[i++] = extra_bit ? '1' : '0';
        bits[i] = 0;

        ff_cbs_trace_syntax_element(ctx, position,
                                    name, subscripts, bits, value);
    }

    *write_to = value;
    return 0;
}