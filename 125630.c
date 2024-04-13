static int cbs_av1_write_leb128(CodedBitstreamContext *ctx, PutBitContext *pbc,
                                const char *name, uint64_t value)
{
    int position, err, len, i;
    uint8_t byte;

    len = (av_log2(value) + 7) / 7;

    if (ctx->trace_enable)
        position = put_bits_count(pbc);

    for (i = 0; i < len; i++) {
        int subscript[2] = { 1, i };

        byte = value >> (7 * i) & 0x7f;
        if (i < len - 1)
            byte |= 0x80;

        err = ff_cbs_write_unsigned(ctx, pbc, 8, "leb128_byte[i]", subscript,
                                    byte, 0x00, 0xff);
        if (err < 0)
            return err;
    }

    if (ctx->trace_enable)
        ff_cbs_trace_syntax_element(ctx, position, name, NULL, "", value);

    return 0;
}