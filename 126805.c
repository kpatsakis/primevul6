static av_cold int decode_init(AVCodecContext *avctx)
{
    FFV1Context *f = avctx->priv_data;
    int ret;

    if ((ret = ffv1_common_init(avctx)) < 0)
        return ret;

    if (avctx->extradata && (ret = read_extra_header(f)) < 0)
        return ret;

    if ((ret = ffv1_init_slice_contexts(f)) < 0)
        return ret;

    avctx->internal->allocate_progress = 1;

    return 0;
}