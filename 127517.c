static int get_coc(Jpeg2000DecoderContext *s, Jpeg2000CodingStyle *c,
                   uint8_t *properties)
{
    int compno, ret;

    if (bytestream2_get_bytes_left(&s->g) < 2)
        return AVERROR_INVALIDDATA;

    compno = bytestream2_get_byteu(&s->g);

    if (compno >= s->ncomponents) {
        av_log(s->avctx, AV_LOG_ERROR,
               "Invalid compno %d. There are %d components in the image.\n",
               compno, s->ncomponents);
        return AVERROR_INVALIDDATA;
    }

    c      += compno;
    c->csty = bytestream2_get_byteu(&s->g);

    if ((ret = get_cox(s, c)) < 0)
        return ret;

    properties[compno] |= HAD_COC;
    return 0;
}