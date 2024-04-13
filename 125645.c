static int cbs_av1_read_leb128(CodedBitstreamContext *ctx, GetBitContext *gbc,
                               const char *name, uint64_t *write_to)
{
    uint64_t value;
    int position, err, i;

    if (ctx->trace_enable)
        position = get_bits_count(gbc);

    value = 0;
    for (i = 0; i < 8; i++) {
        int subscript[2] = { 1, i };
        uint32_t byte;
        err = ff_cbs_read_unsigned(ctx, gbc, 8, "leb128_byte[i]", subscript,
                                   &byte, 0x00, 0xff);
        if (err < 0)
            return err;

        value |= (uint64_t)(byte & 0x7f) << (i * 7);
        if (!(byte & 0x80))
            break;
    }

    if (ctx->trace_enable)
        ff_cbs_trace_syntax_element(ctx, position, name, NULL, "", value);

    *write_to = value;
    return 0;
}