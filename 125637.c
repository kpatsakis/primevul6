static int cbs_av1_write_subexp(CodedBitstreamContext *ctx, PutBitContext *pbc,
                                uint32_t range_max, const char *name,
                                const int *subscripts, uint32_t value)
{
    int position, err;
    uint32_t max_len, len, range_offset, range_bits;

    if (value > range_max) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "%s out of range: "
               "%"PRIu32", but must be in [0,%"PRIu32"].\n",
               name, value, range_max);
        return AVERROR_INVALIDDATA;
    }

    if (ctx->trace_enable)
        position = put_bits_count(pbc);

    av_assert0(range_max > 0);
    max_len = av_log2(range_max - 1) - 3;

    if (value < 8) {
        range_bits   = 3;
        range_offset = 0;
        len = 0;
    } else {
        range_bits = av_log2(value);
        len = range_bits - 2;
        if (len > max_len) {
            // The top bin is combined with the one below it.
            av_assert0(len == max_len + 1);
            --range_bits;
            len = max_len;
        }
        range_offset = 1 << range_bits;
    }

    err = cbs_av1_write_increment(ctx, pbc, 0, max_len,
                                  "subexp_more_bits", len);
    if (err < 0)
        return err;

    if (len < max_len) {
        err = ff_cbs_write_unsigned(ctx, pbc, range_bits,
                                    "subexp_bits", NULL,
                                    value - range_offset,
                                    0, MAX_UINT_BITS(range_bits));
        if (err < 0)
            return err;

    } else {
        err = cbs_av1_write_ns(ctx, pbc, range_max - range_offset,
                               "subexp_final_bits", NULL,
                               value - range_offset);
        if (err < 0)
            return err;
    }

    if (ctx->trace_enable)
        ff_cbs_trace_syntax_element(ctx, position,
                                    name, subscripts, "", value);

    return err;
}