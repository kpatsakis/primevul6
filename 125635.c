static int cbs_av1_write_uvlc(CodedBitstreamContext *ctx, PutBitContext *pbc,
                              const char *name, uint32_t value,
                              uint32_t range_min, uint32_t range_max)
{
    uint32_t v;
    int position, zeroes;

    if (value < range_min || value > range_max) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "%s out of range: "
               "%"PRIu32", but must be in [%"PRIu32",%"PRIu32"].\n",
               name, value, range_min, range_max);
        return AVERROR_INVALIDDATA;
    }

    if (ctx->trace_enable)
        position = put_bits_count(pbc);

    if (value == 0) {
        zeroes = 0;
        put_bits(pbc, 1, 1);
    } else {
        zeroes = av_log2(value + 1);
        v = value - (1 << zeroes) + 1;
        put_bits(pbc, zeroes + 1, 1);
        put_bits(pbc, zeroes, v);
    }

    if (ctx->trace_enable) {
        char bits[65];
        int i, j;
        i = 0;
        for (j = 0; j < zeroes; j++)
            bits[i++] = '0';
        bits[i++] = '1';
        for (j = 0; j < zeroes; j++)
            bits[i++] = (v >> (zeroes - j - 1) & 1) ? '1' : '0';
        bits[i++] = 0;
        ff_cbs_trace_syntax_element(ctx, position, name, NULL,
                                    bits, value);
    }

    return 0;
}