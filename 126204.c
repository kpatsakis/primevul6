AVCodec *avcodec_find_decoder_by_name(const char *name)
{
    AVCodec *p;
    if (!name)
        return NULL;
    p = first_avcodec;
    while (p) {
        if (av_codec_is_decoder(p) && strcmp(name, p->name) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}