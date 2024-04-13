static int reget_buffer_internal(AVCodecContext *avctx, AVFrame *frame)
{
    AVFrame tmp;
    int ret;

    av_assert0(avctx->codec_type == AVMEDIA_TYPE_VIDEO);

    if (frame->data[0] && (frame->width != avctx->width || frame->height != avctx->height || frame->format != avctx->pix_fmt)) {
        av_log(avctx, AV_LOG_WARNING, "Picture changed from size:%dx%d fmt:%s to size:%dx%d fmt:%s in reget buffer()\n",
               frame->width, frame->height, av_get_pix_fmt_name(frame->format), avctx->width, avctx->height, av_get_pix_fmt_name(avctx->pix_fmt));
        av_frame_unref(frame);
    }

    ff_init_buffer_info(avctx, frame);

    if (!frame->data[0])
        return ff_get_buffer(avctx, frame, AV_GET_BUFFER_FLAG_REF);

    if (av_frame_is_writable(frame)) {
        frame->pkt_pts = avctx->internal->pkt ? avctx->internal->pkt->pts : AV_NOPTS_VALUE;
        frame->reordered_opaque = avctx->reordered_opaque;
        return 0;
    }

    av_frame_move_ref(&tmp, frame);

    ret = ff_get_buffer(avctx, frame, AV_GET_BUFFER_FLAG_REF);
    if (ret < 0) {
        av_frame_unref(&tmp);
        return ret;
    }

    av_image_copy(frame->data, frame->linesize, tmp.data, tmp.linesize,
                  frame->format, frame->width, frame->height);

    av_frame_unref(&tmp);

    return 0;
}