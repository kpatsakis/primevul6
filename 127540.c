static int get_qcx(Jpeg2000DecoderContext *s, int n, Jpeg2000QuantStyle *q)
{
    int i, x;

    if (bytestream2_get_bytes_left(&s->g) < 1)
        return AVERROR_INVALIDDATA;

    x = bytestream2_get_byteu(&s->g); // Sqcd

    q->nguardbits = x >> 5;
    q->quantsty   = x & 0x1f;

    if (q->quantsty == JPEG2000_QSTY_NONE) {
        n -= 3;
        if (bytestream2_get_bytes_left(&s->g) < n ||
            n > JPEG2000_MAX_DECLEVELS*3)
            return AVERROR_INVALIDDATA;
        for (i = 0; i < n; i++)
            q->expn[i] = bytestream2_get_byteu(&s->g) >> 3;
    } else if (q->quantsty == JPEG2000_QSTY_SI) {
        if (bytestream2_get_bytes_left(&s->g) < 2)
            return AVERROR_INVALIDDATA;
        x          = bytestream2_get_be16u(&s->g);
        q->expn[0] = x >> 11;
        q->mant[0] = x & 0x7ff;
        for (i = 1; i < JPEG2000_MAX_DECLEVELS * 3; i++) {
            int curexpn = FFMAX(0, q->expn[0] - (i - 1) / 3);
            q->expn[i] = curexpn;
            q->mant[i] = q->mant[0];
        }
    } else {
        n = (n - 3) >> 1;
        if (bytestream2_get_bytes_left(&s->g) < 2 * n ||
            n > JPEG2000_MAX_DECLEVELS*3)
            return AVERROR_INVALIDDATA;
        for (i = 0; i < n; i++) {
            x          = bytestream2_get_be16u(&s->g);
            q->expn[i] = x >> 11;
            q->mant[i] = x & 0x7ff;
        }
    }
    return 0;
}