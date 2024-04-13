static void SetVideoFormat( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;

    p_sys->p_format = schro_decoder_get_video_format(p_sys->p_schro);
    if( p_sys->p_format == NULL ) return;

    p_sys->i_frame_pts_delta = CLOCK_FREQ
                            * p_sys->p_format->frame_rate_denominator
                            / p_sys->p_format->frame_rate_numerator;

    switch( p_sys->p_format->chroma_format )
    {
    case SCHRO_CHROMA_420: p_dec->fmt_out.i_codec = VLC_CODEC_I420; break;
    case SCHRO_CHROMA_422: p_dec->fmt_out.i_codec = VLC_CODEC_I422; break;
    case SCHRO_CHROMA_444: p_dec->fmt_out.i_codec = VLC_CODEC_I444; break;
    default:
        p_dec->fmt_out.i_codec = 0;
        break;
    }

    p_dec->fmt_out.video.i_visible_width = p_sys->p_format->clean_width;
    p_dec->fmt_out.video.i_x_offset = p_sys->p_format->left_offset;
    p_dec->fmt_out.video.i_width = p_sys->p_format->width;

    p_dec->fmt_out.video.i_visible_height = p_sys->p_format->clean_height;
    p_dec->fmt_out.video.i_y_offset = p_sys->p_format->top_offset;
    p_dec->fmt_out.video.i_height = p_sys->p_format->height;

    /* aspect_ratio_[numerator|denominator] describes the pixel aspect ratio */
    p_dec->fmt_out.video.i_sar_num = p_sys->p_format->aspect_ratio_numerator;
    p_dec->fmt_out.video.i_sar_den = p_sys->p_format->aspect_ratio_denominator;

    p_dec->fmt_out.video.i_frame_rate =
        p_sys->p_format->frame_rate_numerator;
    p_dec->fmt_out.video.i_frame_rate_base =
        p_sys->p_format->frame_rate_denominator;
}