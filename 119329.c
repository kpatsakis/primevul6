static int lag_decode_zero_run_line(LagarithContext *l, uint8_t *dst,
                                    const uint8_t *src, const uint8_t *src_end,
                                    int width, int esc_count)
{
    int i = 0;
    int count;
    uint8_t zero_run = 0;
    const uint8_t *src_start = src;
    uint8_t mask1 = -(esc_count < 2);
    uint8_t mask2 = -(esc_count < 3);
    uint8_t *end = dst + (width - 2);

output_zeros:
    if (l->zeros_rem) {
        count = FFMIN(l->zeros_rem, width - i);
        if (end - dst < count) {
            av_log(l->avctx, AV_LOG_ERROR, "Too many zeros remaining.\n");
            return AVERROR_INVALIDDATA;
        }

        memset(dst, 0, count);
        l->zeros_rem -= count;
        dst += count;
    }

    while (dst < end) {
        i = 0;
        while (!zero_run && dst + i < end) {
            i++;
            if (src + i >= src_end)
                return AVERROR_INVALIDDATA;
            zero_run =
                !(src[i] | (src[i + 1] & mask1) | (src[i + 2] & mask2));
        }
        if (zero_run) {
            zero_run = 0;
            i += esc_count;
            memcpy(dst, src, i);
            dst += i;
            l->zeros_rem = lag_calc_zero_run(src[i]);

            src += i + 1;
            goto output_zeros;
        } else {
            memcpy(dst, src, i);
            src += i;
            dst += i;
        }
    }
    return src_start - src;
}