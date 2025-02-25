static int truemotion1_decode_frame(AVCodecContext *avctx,
                                    void *data, int *got_frame,
                                    AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int ret, buf_size = avpkt->size;
    TrueMotion1Context *s = avctx->priv_data;

    s->buf = buf;
    s->size = buf_size;

    if ((ret = truemotion1_decode_header(s)) < 0)
        return ret;

    if ((ret = ff_reget_buffer(avctx, s->frame)) < 0) {
        av_log(s->avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }

    if (compression_types[s->compression].algorithm == ALGO_RGB24H) {
        truemotion1_decode_24bit(s);
    } else if (compression_types[s->compression].algorithm != ALGO_NOP) {
        truemotion1_decode_16bit(s);
    }

    if ((ret = av_frame_ref(data, s->frame)) < 0)
        return ret;

    *got_frame      = 1;

    /* report that the buffer was completely consumed */
    return buf_size;
}