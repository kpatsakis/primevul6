static inline int mpeg4_is_resync(Mpeg4DecContext *ctx)
{
    MpegEncContext *s = &ctx->m;
    int bits_count = get_bits_count(&s->gb);
    int v          = show_bits(&s->gb, 16);

    if (s->workaround_bugs & FF_BUG_NO_PADDING && !ctx->resync_marker)
        return 0;

    while (v <= 0xFF) {
        if (s->pict_type == AV_PICTURE_TYPE_B ||
            (v >> (8 - s->pict_type) != 1) || s->partitioned_frame)
            break;
        skip_bits(&s->gb, 8 + s->pict_type);
        bits_count += 8 + s->pict_type;
        v = show_bits(&s->gb, 16);
    }

    if (bits_count + 8 >= s->gb.size_in_bits) {
        v >>= 8;
        v  |= 0x7F >> (7 - (bits_count & 7));

        if (v == 0x7F)
            return s->mb_num;
    } else {
        if (v == ff_mpeg4_resync_prefix[bits_count & 7]) {
            int len, mb_num;
            int mb_num_bits = av_log2(s->mb_num - 1) + 1;
            GetBitContext gb = s->gb;

            skip_bits(&s->gb, 1);
            align_get_bits(&s->gb);

            for (len = 0; len < 32; len++)
                if (get_bits1(&s->gb))
                    break;

            mb_num = get_bits(&s->gb, mb_num_bits);
            if (!mb_num || mb_num > s->mb_num || get_bits_count(&s->gb)+6 > s->gb.size_in_bits)
                mb_num= -1;

            s->gb = gb;

            if (len >= ff_mpeg4_get_video_packet_prefix_length(s))
                return mb_num;
        }
    }
    return 0;
}