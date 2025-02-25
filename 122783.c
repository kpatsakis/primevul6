static int dnxhd_decode_frame(AVCodecContext *avctx, void *data,
                              int *got_frame, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    DNXHDContext *ctx = avctx->priv_data;
    ThreadFrame frame = { .f = data };
    AVFrame *picture = data;
    int first_field = 1;
    int ret, i;

    ff_dlog(avctx, "frame size %d\n", buf_size);

    for (i = 0; i < avctx->thread_count; i++)
        ctx->rows[i].format = -1;

decode_coding_unit:
    if ((ret = dnxhd_decode_header(ctx, picture, buf, buf_size, first_field)) < 0)
        return ret;

    if ((avctx->width || avctx->height) &&
        (ctx->width != avctx->width || ctx->height != avctx->height)) {
        av_log(avctx, AV_LOG_WARNING, "frame size changed: %dx%d -> %ux%u\n",
               avctx->width, avctx->height, ctx->width, ctx->height);
        first_field = 1;
    }
    if (avctx->pix_fmt != AV_PIX_FMT_NONE && avctx->pix_fmt != ctx->pix_fmt) {
        av_log(avctx, AV_LOG_WARNING, "pix_fmt changed: %s -> %s\n",
               av_get_pix_fmt_name(avctx->pix_fmt), av_get_pix_fmt_name(ctx->pix_fmt));
        first_field = 1;
    }

    avctx->pix_fmt = ctx->pix_fmt;
    ret = ff_set_dimensions(avctx, ctx->width, ctx->height);
    if (ret < 0)
        return ret;

    if (first_field) {
        if ((ret = ff_thread_get_buffer(avctx, &frame, 0)) < 0)
            return ret;
        picture->pict_type = AV_PICTURE_TYPE_I;
        picture->key_frame = 1;
    }

    ctx->buf_size = buf_size - ctx->data_offset;
    ctx->buf = buf + ctx->data_offset;
    avctx->execute2(avctx, dnxhd_decode_row, picture, NULL, ctx->mb_height);

    if (first_field && picture->interlaced_frame) {
        buf      += ctx->cid_table->coding_unit_size;
        buf_size -= ctx->cid_table->coding_unit_size;
        first_field = 0;
        goto decode_coding_unit;
    }

    ret = 0;
    for (i = 0; i < avctx->thread_count; i++) {
        ret += ctx->rows[i].errors;
        ctx->rows[i].errors = 0;
    }

    if (ctx->act) {
        static int act_warned;
        int format = ctx->rows[0].format;
        for (i = 1; i < avctx->thread_count; i++) {
            if (ctx->rows[i].format != format &&
                ctx->rows[i].format != -1 /* not run */) {
                format = 2;
                break;
            }
        }
        switch (format) {
        case -1:
        case 2:
            if (!act_warned) {
                act_warned = 1;
                av_log(ctx->avctx, AV_LOG_ERROR,
                       "Unsupported: variable ACT flag.\n");
            }
            break;
        case 0:
            ctx->pix_fmt = ctx->bit_depth==10
                         ? AV_PIX_FMT_GBRP10 : AV_PIX_FMT_GBRP12;
            break;
        case 1:
            ctx->pix_fmt = ctx->bit_depth==10
                         ? AV_PIX_FMT_YUV444P10 : AV_PIX_FMT_YUV444P12;
            break;
        }
    }
    avctx->pix_fmt = ctx->pix_fmt;
    if (ret) {
        av_log(ctx->avctx, AV_LOG_ERROR, "%d lines with errors\n", ret);
        return AVERROR_INVALIDDATA;
    }

    *got_frame = 1;
    return avpkt->size;
}