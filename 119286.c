static int ref_picture(H264Context *h, Picture *dst, Picture *src)
{
    int ret, i;

    av_assert0(!dst->f.buf[0]);
    av_assert0(src->f.buf[0]);

    src->tf.f = &src->f;
    dst->tf.f = &dst->f;
    ret = ff_thread_ref_frame(&dst->tf, &src->tf);
    if (ret < 0)
        goto fail;

    dst->qscale_table_buf = av_buffer_ref(src->qscale_table_buf);
    dst->mb_type_buf      = av_buffer_ref(src->mb_type_buf);
    if (!dst->qscale_table_buf || !dst->mb_type_buf)
        goto fail;
    dst->qscale_table = src->qscale_table;
    dst->mb_type      = src->mb_type;

    for (i = 0; i < 2; i++) {
        dst->motion_val_buf[i] = av_buffer_ref(src->motion_val_buf[i]);
        dst->ref_index_buf[i]  = av_buffer_ref(src->ref_index_buf[i]);
        if (!dst->motion_val_buf[i] || !dst->ref_index_buf[i])
            goto fail;
        dst->motion_val[i] = src->motion_val[i];
        dst->ref_index[i]  = src->ref_index[i];
    }

    if (src->hwaccel_picture_private) {
        dst->hwaccel_priv_buf = av_buffer_ref(src->hwaccel_priv_buf);
        if (!dst->hwaccel_priv_buf)
            goto fail;
        dst->hwaccel_picture_private = dst->hwaccel_priv_buf->data;
    }

    for (i = 0; i < 2; i++)
        dst->field_poc[i] = src->field_poc[i];

    memcpy(dst->ref_poc,   src->ref_poc,   sizeof(src->ref_poc));
    memcpy(dst->ref_count, src->ref_count, sizeof(src->ref_count));

    dst->poc           = src->poc;
    dst->frame_num     = src->frame_num;
    dst->mmco_reset    = src->mmco_reset;
    dst->pic_id        = src->pic_id;
    dst->long_ref      = src->long_ref;
    dst->mbaff         = src->mbaff;
    dst->field_picture = src->field_picture;
    dst->needs_realloc = src->needs_realloc;
    dst->reference     = src->reference;
    dst->crop          = src->crop;
    dst->crop_left     = src->crop_left;
    dst->crop_top      = src->crop_top;
    dst->recovered     = src->recovered;

    return 0;
fail:
    unref_picture(h, dst);
    return ret;
}