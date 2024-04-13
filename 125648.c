static int cbs_av1_read_increment(CodedBitstreamContext *ctx, GetBitContext *gbc,
                                  uint32_t range_min, uint32_t range_max,
                                  const char *name, uint32_t *write_to)
{
    uint32_t value;
    int position, i;
    char bits[33];

    av_assert0(range_min <= range_max && range_max - range_min < sizeof(bits) - 1);
    if (ctx->trace_enable)
        position = get_bits_count(gbc);

    for (i = 0, value = range_min; value < range_max;) {
        if (get_bits_left(gbc) < 1) {
            av_log(ctx->log_ctx, AV_LOG_ERROR, "Invalid increment value at "
                   "%s: bitstream ended.\n", name);
            return AVERROR_INVALIDDATA;
        }
        if (get_bits1(gbc)) {
            bits[i++] = '1';
            ++value;
        } else {
            bits[i++] = '0';
            break;
        }
    }

    if (ctx->trace_enable) {
        bits[i] = 0;
        ff_cbs_trace_syntax_element(ctx, position,
                                    name, NULL, bits, value);
    }

    *write_to = value;
    return 0;
}