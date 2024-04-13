static int cbs_av1_read_su(CodedBitstreamContext *ctx, GetBitContext *gbc,
                           int width, const char *name,
                           const int *subscripts, int32_t *write_to)
{
    int position;
    int32_t value;

    if (ctx->trace_enable)
        position = get_bits_count(gbc);

    if (get_bits_left(gbc) < width) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "Invalid signed value at "
               "%s: bitstream ended.\n", name);
        return AVERROR_INVALIDDATA;
    }

    value = get_sbits(gbc, width);

    if (ctx->trace_enable) {
        char bits[33];
        int i;
        for (i = 0; i < width; i++)
            bits[i] = value & (1 << (width - i - 1)) ? '1' : '0';
        bits[i] = 0;

        ff_cbs_trace_syntax_element(ctx, position,
                                    name, subscripts, bits, value);
    }

    *write_to = value;
    return 0;
}