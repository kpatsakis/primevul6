static int cbs_av1_write_su(CodedBitstreamContext *ctx, PutBitContext *pbc,
                            int width, const char *name,
                            const int *subscripts, int32_t value)
{
    if (put_bits_left(pbc) < width)
        return AVERROR(ENOSPC);

    if (ctx->trace_enable) {
        char bits[33];
        int i;
        for (i = 0; i < width; i++)
            bits[i] = value & (1 << (width - i - 1)) ? '1' : '0';
        bits[i] = 0;

        ff_cbs_trace_syntax_element(ctx, put_bits_count(pbc),
                                    name, subscripts, bits, value);
    }

    put_sbits(pbc, width, value);

    return 0;
}