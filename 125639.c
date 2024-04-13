static int cbs_av1_write_unit(CodedBitstreamContext *ctx,
                              CodedBitstreamUnit *unit)
{
    CodedBitstreamAV1Context *priv = ctx->priv_data;
    PutBitContext pbc;
    int err;

    if (!priv->write_buffer) {
        // Initial write buffer size is 1MB.
        priv->write_buffer_size = 1024 * 1024;

    reallocate_and_try_again:
        err = av_reallocp(&priv->write_buffer, priv->write_buffer_size);
        if (err < 0) {
            av_log(ctx->log_ctx, AV_LOG_ERROR, "Unable to allocate a "
                   "sufficiently large write buffer (last attempt "
                   "%"SIZE_SPECIFIER" bytes).\n", priv->write_buffer_size);
            return err;
        }
    }

    init_put_bits(&pbc, priv->write_buffer, priv->write_buffer_size);

    err = cbs_av1_write_obu(ctx, unit, &pbc);
    if (err == AVERROR(ENOSPC)) {
        // Overflow.
        priv->write_buffer_size *= 2;
        goto reallocate_and_try_again;
    }
    if (err < 0)
        return err;

    // Overflow but we didn't notice.
    av_assert0(put_bits_count(&pbc) <= 8 * priv->write_buffer_size);

    // OBU data must be byte-aligned.
    av_assert0(put_bits_count(&pbc) % 8 == 0);

    unit->data_size = put_bits_count(&pbc) / 8;
    flush_put_bits(&pbc);

    err = ff_cbs_alloc_unit_data(ctx, unit, unit->data_size);
    if (err < 0)
        return err;

    memcpy(unit->data, priv->write_buffer, unit->data_size);

    return 0;
}