static int unsupported_codec(AVFormatContext *s,
                             const char* type, int codec_id)
{
    const AVCodecDescriptor *desc = avcodec_descriptor_get(codec_id);
    av_log(s, AV_LOG_ERROR,
           "%s codec %s not compatible with flv\n",
            type,
            desc ? desc->name : "unknown");
    return AVERROR(ENOSYS);
}