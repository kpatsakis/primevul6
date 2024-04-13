static int cbs_av1_write_increment(CodedBitstreamContext *ctx, PutBitContext *pbc,
                                   uint32_t range_min, uint32_t range_max,
                                   const char *name, uint32_t value)
{
    int len;

    av_assert0(range_min <= range_max && range_max - range_min < 32);
    if (value < range_min || value > range_max) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "%s out of range: "
               "%"PRIu32", but must be in [%"PRIu32",%"PRIu32"].\n",
               name, value, range_min, range_max);
        return AVERROR_INVALIDDATA;
    }

    if (value == range_max)
        len = range_max - range_min;
    else
        len = value - range_min + 1;
    if (put_bits_left(pbc) < len)
        return AVERROR(ENOSPC);

    if (ctx->trace_enable) {
        char bits[33];
        int i;
        for (i = 0; i < len; i++) {
            if (range_min + i == value)
                bits[i] = '0';
            else
                bits[i] = '1';
        }
        bits[i] = 0;
        ff_cbs_trace_syntax_element(ctx, put_bits_count(pbc),
                                    name, NULL, bits, value);
    }

    if (len > 0)
        put_bits(pbc, len, (1 << len) - 1 - (value != range_max));

    return 0;
}