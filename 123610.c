static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    SchroDecoder *p_schro;

    if( p_dec->fmt_in.i_codec != VLC_CODEC_DIRAC )
    {
        return VLC_EGENERIC;
    }

    /* Allocate the memory needed to store the decoder's structure */
    p_sys = malloc(sizeof(decoder_sys_t));
    if( p_sys == NULL )
        return VLC_ENOMEM;

    /* Initialise the schroedinger (and hence liboil libraries */
    /* This does no allocation and is safe to call */
    schro_init();

    /* Initialise the schroedinger decoder */
    if( !(p_schro = schro_decoder_new()) )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }

    p_dec->p_sys = p_sys;
    p_sys->p_schro = p_schro;
    p_sys->p_format = NULL;
    p_sys->i_lastpts = VLC_TS_INVALID;
    p_sys->i_frame_pts_delta = 0;

    /* Set output properties */
    p_dec->fmt_out.i_cat = VIDEO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_I420;

    /* Set callbacks */
    p_dec->pf_decode_video = DecodeBlock;

    return VLC_SUCCESS;
}