static int cbs_av1_split_fragment(CodedBitstreamContext *ctx,
                                  CodedBitstreamFragment *frag,
                                  int header)
{
    GetBitContext gbc;
    uint8_t *data;
    size_t size;
    uint64_t obu_length;
    int pos, err, trace;

    // Don't include this parsing in trace output.
    trace = ctx->trace_enable;
    ctx->trace_enable = 0;

    data = frag->data;
    size = frag->data_size;

    if (INT_MAX / 8 < size) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "Invalid fragment: "
               "too large (%"SIZE_SPECIFIER" bytes).\n", size);
        err = AVERROR_INVALIDDATA;
        goto fail;
    }

    while (size > 0) {
        AV1RawOBUHeader header;
        uint64_t obu_size;

        init_get_bits(&gbc, data, 8 * size);

        err = cbs_av1_read_obu_header(ctx, &gbc, &header);
        if (err < 0)
            goto fail;

        if (get_bits_left(&gbc) < 8) {
            av_log(ctx->log_ctx, AV_LOG_ERROR, "Invalid OBU: fragment "
                   "too short (%"SIZE_SPECIFIER" bytes).\n", size);
            err = AVERROR_INVALIDDATA;
            goto fail;
        }

        if (header.obu_has_size_field) {
            err = cbs_av1_read_leb128(ctx, &gbc, "obu_size", &obu_size);
            if (err < 0)
                goto fail;
        } else
            obu_size = size - 1 - header.obu_extension_flag;

        pos = get_bits_count(&gbc);
        av_assert0(pos % 8 == 0 && pos / 8 <= size);

        obu_length = pos / 8 + obu_size;

        if (size < obu_length) {
            av_log(ctx->log_ctx, AV_LOG_ERROR, "Invalid OBU length: "
                   "%"PRIu64", but only %"SIZE_SPECIFIER" bytes remaining in fragment.\n",
                   obu_length, size);
            err = AVERROR_INVALIDDATA;
            goto fail;
        }

        err = ff_cbs_insert_unit_data(ctx, frag, -1, header.obu_type,
                                      data, obu_length, frag->data_ref);
        if (err < 0)
            goto fail;

        data += obu_length;
        size -= obu_length;
    }

    err = 0;
fail:
    ctx->trace_enable = trace;
    return err;
}