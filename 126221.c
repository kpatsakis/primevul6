AVCodec *avcodec_find_encoder_by_name(const char *name)
{
    AVCodec *p;
    if (!name)
        return NULL;
    p = first_avcodec;
    while (p) {
        if (av_codec_is_encoder(p) && strcmp(name, p->name) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}