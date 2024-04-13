static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    int i_tmp;
    float f_tmp;
    char *psz_tmp;

    if( p_enc->fmt_out.i_codec != VLC_CODEC_DIRAC &&
        !p_enc->b_force )
    {
        return VLC_EGENERIC;
    }

    if( !p_enc->fmt_in.video.i_frame_rate || !p_enc->fmt_in.video.i_frame_rate_base ||
        !p_enc->fmt_in.video.i_visible_height || !p_enc->fmt_in.video.i_visible_width )
    {
        msg_Err( p_enc, "Framerate and picture dimensions must be non-zero" );
        return VLC_EGENERIC;
    }

    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_sys = calloc( 1, sizeof( *p_sys ) ) ) == NULL )
        return VLC_ENOMEM;

    p_enc->p_sys = p_sys;
    p_enc->pf_encode_video = Encode;
    p_enc->fmt_out.i_codec = VLC_CODEC_DIRAC;
    p_enc->fmt_out.i_cat = VIDEO_ES;

    if( ( p_sys->p_dts_fifo = block_FifoNew() ) == NULL )
    {
        CloseEncoder( p_this );
        return VLC_ENOMEM;
    }

    ResetPTStlb( p_enc );

    /* guess the video format based upon number of lines and picture height */
    int i = 0;
    SchroVideoFormatEnum guessed_video_fmt = SCHRO_VIDEO_FORMAT_CUSTOM;
    /* Pick the dirac_video_format in this order of preference:
     *  1. an exact match in frame height and an approximate fps match
     *  2. the previous preset with a smaller number of lines.
     */
    do
    {
        if( schro_format_guess[i].i_height > p_enc->fmt_in.video.i_height )
        {
            guessed_video_fmt = schro_format_guess[i-1].i_vf;
            break;
        }
        if( schro_format_guess[i].i_height != p_enc->fmt_in.video.i_height )
            continue;
        int src_fps = p_enc->fmt_in.video.i_frame_rate / p_enc->fmt_in.video.i_frame_rate_base;
        int delta_fps = abs( schro_format_guess[i].i_approx_fps - src_fps );
        if( delta_fps > 2 )
            continue;

        guessed_video_fmt = schro_format_guess[i].i_vf;
        break;
    } while( schro_format_guess[++i].i_height );

    schro_init();
    p_sys->p_schro = schro_encoder_new();
    if( !p_sys->p_schro ) {
        msg_Err( p_enc, "Failed to initialize libschroedinger encoder" );
        return VLC_EGENERIC;
    }
    schro_encoder_set_packet_assembly( p_sys->p_schro, true );

    if( !( p_sys->p_format = schro_encoder_get_video_format( p_sys->p_schro ) ) ) {
        msg_Err( p_enc, "Failed to get Schroedigner video format" );
        schro_encoder_free( p_sys->p_schro );
        return VLC_EGENERIC;
    }

    /* initialise the video format parameters to the guessed format */
    schro_video_format_set_std_video_format( p_sys->p_format, guessed_video_fmt );

    /* constants set from the input video format */
    p_sys->p_format->width                  = p_enc->fmt_in.video.i_visible_width;
    p_sys->p_format->height                 = p_enc->fmt_in.video.i_visible_height;
    p_sys->p_format->frame_rate_numerator   = p_enc->fmt_in.video.i_frame_rate;
    p_sys->p_format->frame_rate_denominator = p_enc->fmt_in.video.i_frame_rate_base;
    unsigned u_asr_num, u_asr_den;
    vlc_ureduce( &u_asr_num, &u_asr_den,
                 p_enc->fmt_in.video.i_sar_num,
                 p_enc->fmt_in.video.i_sar_den, 0 );
    p_sys->p_format->aspect_ratio_numerator   = u_asr_num;
    p_sys->p_format->aspect_ratio_denominator = u_asr_den;

    config_ChainParse( p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg );

    SCHRO_SET_ENUM(enc_rate_control_list, ENC_RATE_CONTROL, ENC_RATE_CONTROL_TEXT, "rate_control")

    SCHRO_SET_ENUM(enc_gop_structure_list, ENC_GOP_STRUCTURE, ENC_GOP_STRUCTURE_TEXT, "gop_structure")

    psz_tmp = var_GetString( p_enc, ENC_CFG_PREFIX ENC_CHROMAFMT );
    if( !psz_tmp )
        goto error;
    else {
        uint32_t i_codec;
        if( !strcmp( psz_tmp, "420" ) ) {
            i_codec = VLC_CODEC_I420;
        }
        else if( !strcmp( psz_tmp, "422" ) ) {
            i_codec = VLC_CODEC_I422;
        }
        else if( !strcmp( psz_tmp, "444" ) ) {
            i_codec = VLC_CODEC_I444;
        }
        else {
            msg_Err( p_enc, "Invalid chroma format: %s", psz_tmp );
            free( psz_tmp );
            goto error;
        }
        SetEncChromaFormat( p_enc, i_codec );
    }
    free( psz_tmp );

    SCHRO_SET_FLOAT(ENC_QUALITY, "quality")

    SCHRO_SET_FLOAT(ENC_NOISE_THRESHOLD, "noise_threshold")

    /* use bitrate from sout-transcode-vb in kbps */
    i_tmp = var_GetInteger( p_enc, ENC_CFG_PREFIX ENC_BITRATE );
    if( i_tmp > -1 )
        schro_encoder_setting_set_double( p_sys->p_schro, "bitrate", i_tmp * 1000 );
    else
        schro_encoder_setting_set_double( p_sys->p_schro, "bitrate", p_enc->fmt_out.i_bitrate );

       p_enc->fmt_out.i_bitrate = schro_encoder_setting_get_double( p_sys->p_schro, "bitrate" );

    i_tmp = var_GetInteger( p_enc, ENC_CFG_PREFIX ENC_MIN_BITRATE );
    if( i_tmp > -1 )
        schro_encoder_setting_set_double( p_sys->p_schro, "min_bitrate", i_tmp * 1000 );

    i_tmp = var_GetInteger( p_enc, ENC_CFG_PREFIX ENC_MAX_BITRATE );
    if( i_tmp > -1 )
        schro_encoder_setting_set_double( p_sys->p_schro, "max_bitrate", i_tmp * 1000 );

    SCHRO_SET_INTEGER(ENC_AU_DISTANCE, "au_distance", -1)

    SCHRO_SET_ENUM(enc_filtering_list, ENC_PREFILTER, ENC_PREFILTER_TEXT, "filtering")

    SCHRO_SET_FLOAT(ENC_PREFILTER_STRENGTH, "filter_value")


    psz_tmp = var_GetString( p_enc, ENC_CFG_PREFIX ENC_CODINGMODE );
    if( !psz_tmp )
        goto error;
    else if( !strcmp( psz_tmp, "auto" ) ) {
        p_sys->b_auto_field_coding = true;
    }
    else if( !strcmp( psz_tmp, "progressive" ) ) {
        p_sys->b_auto_field_coding = false;
        schro_encoder_setting_set_double( p_sys->p_schro, "interlaced_coding", false);
    }
    else if( !strcmp( psz_tmp, "field" ) ) {
        p_sys->b_auto_field_coding = false;
        schro_encoder_setting_set_double( p_sys->p_schro, "interlaced_coding", true);
    }
    else {
        msg_Err( p_enc, "Invalid codingmode: %s", psz_tmp );
        free( psz_tmp );
        goto error;
    }
    free( psz_tmp );

    SCHRO_SET_ENUM(enc_block_size_list, ENC_MCBLK_SIZE, ENC_MCBLK_SIZE_TEXT, "motion_block_size")

    SCHRO_SET_ENUM(enc_block_overlap_list, ENC_MCBLK_OVERLAP, ENC_MCBLK_OVERLAP_TEXT, "motion_block_overlap")

    psz_tmp = var_GetString( p_enc, ENC_CFG_PREFIX ENC_MVPREC );
    if( !psz_tmp )
        goto error;
    else if( *psz_tmp != '\0') {
        if( !strcmp( psz_tmp, "1" ) ) {
            schro_encoder_setting_set_double( p_sys->p_schro, "mv_precision", 0 );
        }
        else if( !strcmp( psz_tmp, "1/2" ) ) {
            schro_encoder_setting_set_double( p_sys->p_schro, "mv_precision", 1 );
        }
        else if( !strcmp( psz_tmp, "1/4" ) ) {
            schro_encoder_setting_set_double( p_sys->p_schro, "mv_precision", 2 );
        }
        else if( !strcmp( psz_tmp, "1/8" ) ) {
            schro_encoder_setting_set_double( p_sys->p_schro, "mv_precision", 3 );
        }
        else {
            msg_Err( p_enc, "Invalid mv_precision: %s", psz_tmp );
            free( psz_tmp );
            goto error;
        }
    }
    free( psz_tmp );

    SCHRO_SET_INTEGER(ENC_ME_COMBINED, "enable_chroma_me", -1)

    SCHRO_SET_ENUM(enc_wavelet_list, ENC_DWTINTRA, ENC_DWTINTRA_TEXT, "intra_wavelet")

    SCHRO_SET_ENUM(enc_wavelet_list, ENC_DWTINTER, ENC_DWTINTER_TEXT, "inter_wavelet")

    SCHRO_SET_INTEGER(ENC_DWTDEPTH, "transform_depth", -1)

    SCHRO_SET_INTEGER(ENC_MULTIQUANT, "enable_multiquant", -1)

    SCHRO_SET_INTEGER(ENC_NOAC, "enable_noarith", -1)

    SCHRO_SET_ENUM(enc_perceptual_weighting_list, ENC_PWT, ENC_PWT_TEXT, "perceptual_weighting")

    SCHRO_SET_FLOAT(ENC_PDIST, "perceptual_distance")

    SCHRO_SET_INTEGER(ENC_HSLICES, "horiz_slices", -1)

    SCHRO_SET_INTEGER(ENC_VSLICES, "vert_slices", -1)

    SCHRO_SET_ENUM(enc_codeblock_size_list, ENC_SCBLK_SIZE, ENC_SCBLK_SIZE_TEXT, "codeblock_size")

    SCHRO_SET_INTEGER(ENC_ME_HIERARCHICAL, "enable_hierarchical_estimation", -1)

    SCHRO_SET_INTEGER(ENC_ME_DOWNSAMPLE_LEVELS, "downsample_levels", 1)

    SCHRO_SET_INTEGER(ENC_ME_GLOBAL_MOTION, "enable_global_motion", -1)

    SCHRO_SET_INTEGER(ENC_ME_PHASECORR, "enable_phasecorr_estimation", -1)

    SCHRO_SET_INTEGER(ENC_SCD, "enable_scene_change_detection", -1)

    SCHRO_SET_ENUM(enc_profile_list, ENC_FORCE_PROFILE, ENC_FORCE_PROFILE_TEXT, "force_profile")

    p_sys->started = 0;

    return VLC_SUCCESS;
error:
    CloseEncoder( p_this );
    return VLC_EGENERIC;
}