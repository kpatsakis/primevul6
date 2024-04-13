static bool SetEncChromaFormat( encoder_t *p_enc, uint32_t i_codec )
{
    encoder_sys_t *p_sys = p_enc->p_sys;

    switch( i_codec ) {
    case VLC_CODEC_I420:
        p_enc->fmt_in.i_codec = i_codec;
        p_enc->fmt_in.video.i_bits_per_pixel = 12;
        p_sys->p_format->chroma_format = SCHRO_CHROMA_420;
           break;
    case VLC_CODEC_I422:
        p_enc->fmt_in.i_codec = i_codec;
        p_enc->fmt_in.video.i_bits_per_pixel = 16;
        p_sys->p_format->chroma_format = SCHRO_CHROMA_422;
        break;
    case VLC_CODEC_I444:
        p_enc->fmt_in.i_codec = i_codec;
        p_enc->fmt_in.video.i_bits_per_pixel = 24;
        p_sys->p_format->chroma_format = SCHRO_CHROMA_444;
        break;
    default:
        return false;
    }

    return true;
}