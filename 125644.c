static void cbs_av1_close(CodedBitstreamContext *ctx)
{
    CodedBitstreamAV1Context *priv = ctx->priv_data;

    av_buffer_unref(&priv->sequence_header_ref);
    av_buffer_unref(&priv->frame_header_ref);

    av_freep(&priv->write_buffer);
}