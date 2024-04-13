static int cbs_av1_ref_tile_data(CodedBitstreamContext *ctx,
                                 CodedBitstreamUnit *unit,
                                 GetBitContext *gbc,
                                 AV1RawTileData *td)
{
    int pos;

    pos = get_bits_count(gbc);
    if (pos >= 8 * unit->data_size) {
        av_log(ctx->log_ctx, AV_LOG_ERROR, "Bitstream ended before "
               "any data in tile group (%d bits read).\n", pos);
        return AVERROR_INVALIDDATA;
    }
    // Must be byte-aligned at this point.
    av_assert0(pos % 8 == 0);

    td->data_ref = av_buffer_ref(unit->data_ref);
    if (!td->data_ref)
        return AVERROR(ENOMEM);

    td->data      = unit->data      + pos / 8;
    td->data_size = unit->data_size - pos / 8;

    return 0;
}