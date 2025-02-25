static int h264_set_parameter_from_sps(H264Context *h)
{
    if (h->flags & CODEC_FLAG_LOW_DELAY ||
        (h->sps.bitstream_restriction_flag &&
         !h->sps.num_reorder_frames)) {
        if (h->avctx->has_b_frames > 1 || h->delayed_pic[0])
            av_log(h->avctx, AV_LOG_WARNING, "Delayed frames seen. "
                   "Reenabling low delay requires a codec flush.\n");
        else
            h->low_delay = 1;
    }

    if (h->avctx->has_b_frames < 2)
        h->avctx->has_b_frames = !h->low_delay;

    if (h->avctx->bits_per_raw_sample != h->sps.bit_depth_luma ||
        h->cur_chroma_format_idc      != h->sps.chroma_format_idc) {
        if (h->avctx->codec &&
            h->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU &&
            (h->sps.bit_depth_luma != 8 || h->sps.chroma_format_idc > 1)) {
            av_log(h->avctx, AV_LOG_ERROR,
                   "VDPAU decoding does not support video colorspace.\n");
            return AVERROR_INVALIDDATA;
        }
        if (h->sps.bit_depth_luma >= 8 && h->sps.bit_depth_luma <= 14 &&
            h->sps.bit_depth_luma != 11 && h->sps.bit_depth_luma != 13) {
            h->avctx->bits_per_raw_sample = h->sps.bit_depth_luma;
            h->cur_chroma_format_idc      = h->sps.chroma_format_idc;
            h->pixel_shift                = h->sps.bit_depth_luma > 8;

            ff_h264dsp_init(&h->h264dsp, h->sps.bit_depth_luma,
                            h->sps.chroma_format_idc);
            ff_h264chroma_init(&h->h264chroma, h->sps.bit_depth_chroma);
            ff_h264qpel_init(&h->h264qpel, h->sps.bit_depth_luma);
            ff_h264_pred_init(&h->hpc, h->avctx->codec_id, h->sps.bit_depth_luma,
                              h->sps.chroma_format_idc);

            if (CONFIG_ERROR_RESILIENCE)
                ff_dsputil_init(&h->dsp, h->avctx);
            ff_videodsp_init(&h->vdsp, h->sps.bit_depth_luma);
        } else {
            av_log(h->avctx, AV_LOG_ERROR, "Unsupported bit depth %d\n",
                   h->sps.bit_depth_luma);
            return AVERROR_INVALIDDATA;
        }
    }
    return 0;
}