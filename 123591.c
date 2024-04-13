int rtp_get_fmt( vlc_object_t *obj, es_format_t *p_fmt, const char *mux,
                  rtp_format_t *rtp_fmt )
{
    assert( p_fmt != NULL || mux != NULL );

    /* Dynamic payload type. Payload types are scoped to the RTP
     * session, and we put each ES in its own session, so no risk of
     * conflict. */
    rtp_fmt->payload_type = 96;
    rtp_fmt->cat = mux != NULL ? VIDEO_ES : p_fmt->i_cat;
    if( rtp_fmt->cat == AUDIO_ES )
    {
        rtp_fmt->clock_rate = p_fmt->audio.i_rate;
        rtp_fmt->channels = p_fmt->audio.i_channels;
    }
    else
        rtp_fmt->clock_rate = 90000; /* most common case for video */
    /* Stream bitrate in kbps */
    rtp_fmt->bitrate = p_fmt != NULL ? p_fmt->i_bitrate/1000 : 0;
    rtp_fmt->fmtp = NULL;

    if( mux != NULL )
    {
        if( strncmp( mux, "ts", 2 ) == 0 )
        {
            rtp_fmt->payload_type = 33;
            rtp_fmt->ptname = "MP2T";
        }
        else
            rtp_fmt->ptname = "MP2P";
        return VLC_SUCCESS;
    }

    switch( p_fmt->i_codec )
    {
        case VLC_CODEC_MULAW:
            if( p_fmt->audio.i_channels == 1 && p_fmt->audio.i_rate == 8000 )
                rtp_fmt->payload_type = 0;
            rtp_fmt->ptname = "PCMU";
            rtp_fmt->pf_packetize = rtp_packetize_pcm;
            break;
        case VLC_CODEC_ALAW:
            if( p_fmt->audio.i_channels == 1 && p_fmt->audio.i_rate == 8000 )
                rtp_fmt->payload_type = 8;
            rtp_fmt->ptname = "PCMA";
            rtp_fmt->pf_packetize = rtp_packetize_pcm;
            break;
        case VLC_CODEC_S16B:
        case VLC_CODEC_S16L:
            if( p_fmt->audio.i_channels == 1 && p_fmt->audio.i_rate == 44100 )
            {
                rtp_fmt->payload_type = 11;
            }
            else if( p_fmt->audio.i_channels == 2 &&
                     p_fmt->audio.i_rate == 44100 )
            {
                rtp_fmt->payload_type = 10;
            }
            rtp_fmt->ptname = "L16";
            if( p_fmt->i_codec == VLC_CODEC_S16B )
                rtp_fmt->pf_packetize = rtp_packetize_pcm;
            else
                rtp_fmt->pf_packetize = rtp_packetize_swab;
            break;
        case VLC_CODEC_U8:
            rtp_fmt->ptname = "L8";
            rtp_fmt->pf_packetize = rtp_packetize_pcm;
            break;
        case VLC_CODEC_S24B:
            rtp_fmt->ptname = "L24";
            rtp_fmt->pf_packetize = rtp_packetize_pcm;
            break;
        case VLC_CODEC_MPGA:
            rtp_fmt->payload_type = 14;
            rtp_fmt->ptname = "MPA";
            rtp_fmt->clock_rate = 90000; /* not 44100 */
            rtp_fmt->pf_packetize = rtp_packetize_mpa;
            break;
        case VLC_CODEC_MPGV:
            rtp_fmt->payload_type = 32;
            rtp_fmt->ptname = "MPV";
            rtp_fmt->pf_packetize = rtp_packetize_mpv;
            break;
        case VLC_CODEC_ADPCM_G726:
            switch( p_fmt->i_bitrate / 1000 )
            {
            case 16:
                rtp_fmt->ptname = "G726-16";
                rtp_fmt->pf_packetize = rtp_packetize_g726_16;
                break;
            case 24:
                rtp_fmt->ptname = "G726-24";
                rtp_fmt->pf_packetize = rtp_packetize_g726_24;
                break;
            case 32:
                rtp_fmt->ptname = "G726-32";
                rtp_fmt->pf_packetize = rtp_packetize_g726_32;
                break;
            case 40:
                rtp_fmt->ptname = "G726-40";
                rtp_fmt->pf_packetize = rtp_packetize_g726_40;
                break;
            default:
                msg_Err( obj, "cannot add this stream (unsupported "
                         "G.726 bit rate: %u)", p_fmt->i_bitrate );
                return VLC_EGENERIC;
            }
            break;
        case VLC_CODEC_A52:
            rtp_fmt->ptname = "ac3";
            rtp_fmt->pf_packetize = rtp_packetize_ac3;
            break;
        case VLC_CODEC_H263:
            rtp_fmt->ptname = "H263-1998";
            rtp_fmt->pf_packetize = rtp_packetize_h263;
            break;
        case VLC_CODEC_H264:
            rtp_fmt->ptname = "H264";
            rtp_fmt->pf_packetize = rtp_packetize_h264;
            rtp_fmt->fmtp = NULL;

            if( p_fmt->i_extra > 0 )
            {
                uint8_t *p_buffer = p_fmt->p_extra;
                int     i_buffer = p_fmt->i_extra;
                char    *p_64_sps = NULL;
                char    *p_64_pps = NULL;
                char    hexa[6+1];

                while( i_buffer > 4 )
                {
                    int i_offset    = 0;
                    int i_size      = 0;

                    while( p_buffer[0] != 0 || p_buffer[1] != 0 ||
                           p_buffer[2] != 1 )
                    {
                        p_buffer++;
                        i_buffer--;
                        if( i_buffer == 0 ) break;
                    }

                    if( i_buffer < 4 || memcmp(p_buffer, "\x00\x00\x01", 3 ) )
                    {
                        msg_Dbg( obj, "No startcode found..");
                        break;
                    }
                    p_buffer += 3;
                    i_buffer -= 3;

                    const int i_nal_type = p_buffer[0]&0x1f;

                    msg_Dbg( obj, "we found a startcode for NAL with TYPE:%d", i_nal_type );

                    i_size = i_buffer;
                    for( i_offset = 0; i_offset+2 < i_buffer ; i_offset++)
                    {
                        if( !memcmp(p_buffer + i_offset, "\x00\x00\x01", 3 ) )
                        {
                            /* we found another startcode */
                            while( i_offset > 0 && 0 == p_buffer[ i_offset - 1 ] )
                                i_offset--;
                            i_size = i_offset;
                            break;
                        }
                    }

                    if( i_size == 0 )
                    {
                        msg_Dbg( obj, "No-info found in nal ");
                        continue;
                    }

                    if( i_nal_type == 7 )
                    {
                        free( p_64_sps );
                        p_64_sps = vlc_b64_encode_binary( p_buffer, i_size );
                        /* XXX: nothing ensures that i_size >= 4 ?? */
                        sprintf_hexa( hexa, &p_buffer[1], 3 );
                    }
                    else if( i_nal_type == 8 )
                    {
                        free( p_64_pps );
                        p_64_pps = vlc_b64_encode_binary( p_buffer, i_size );
                    }
                    i_buffer -= i_size;
                    p_buffer += i_size;
                }
                /* */
                if( p_64_sps && p_64_pps &&
                    ( asprintf( &rtp_fmt->fmtp,
                                "packetization-mode=1;profile-level-id=%s;"
                                "sprop-parameter-sets=%s,%s;", hexa, p_64_sps,
                                p_64_pps ) == -1 ) )
                    rtp_fmt->fmtp = NULL;
                free( p_64_sps );
                free( p_64_pps );
            }
            if( rtp_fmt->fmtp == NULL )
                rtp_fmt->fmtp = strdup( "packetization-mode=1" );
            break;

        case VLC_CODEC_MP4V:
        {
            rtp_fmt->ptname = "MP4V-ES";
            rtp_fmt->pf_packetize = rtp_packetize_split;
            if( p_fmt->i_extra > 0 )
            {
                char hexa[2*p_fmt->i_extra +1];
                sprintf_hexa( hexa, p_fmt->p_extra, p_fmt->i_extra );
                if( asprintf( &rtp_fmt->fmtp,
                              "profile-level-id=3; config=%s;", hexa ) == -1 )
                    rtp_fmt->fmtp = NULL;
            }
            break;
        }
        case VLC_CODEC_MP4A:
        {
            if( ! var_InheritBool( obj, "sout-rtp-mp4a-latm" ) )
            {
                char hexa[2*p_fmt->i_extra +1];

                rtp_fmt->ptname = "mpeg4-generic";
                rtp_fmt->pf_packetize = rtp_packetize_mp4a;
                sprintf_hexa( hexa, p_fmt->p_extra, p_fmt->i_extra );
                if( asprintf( &rtp_fmt->fmtp,
                              "streamtype=5; profile-level-id=15; "
                              "mode=AAC-hbr; config=%s; SizeLength=13; "
                              "IndexLength=3; IndexDeltaLength=3; Profile=1;",
                              hexa ) == -1 )
                    rtp_fmt->fmtp = NULL;
            }
            else
            {
                char hexa[13];
                int i;
                unsigned char config[6];
                unsigned int aacsrates[15] = {
                    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
                    16000, 12000, 11025, 8000, 7350, 0, 0 };

                for( i = 0; i < 15; i++ )
                    if( p_fmt->audio.i_rate == aacsrates[i] )
                        break;

                config[0]=0x40;
                config[1]=0;
                config[2]=0x20|i;
                config[3]=p_fmt->audio.i_channels<<4;
                config[4]=0x3f;
                config[5]=0xc0;

                rtp_fmt->ptname = "MP4A-LATM";
                rtp_fmt->pf_packetize = rtp_packetize_mp4a_latm;
                sprintf_hexa( hexa, config, 6 );
                if( asprintf( &rtp_fmt->fmtp, "profile-level-id=15; "
                              "object=2; cpresent=0; config=%s", hexa ) == -1 )
                    rtp_fmt->fmtp = NULL;
            }
            break;
        }
        case VLC_CODEC_AMR_NB:
            rtp_fmt->ptname = "AMR";
            rtp_fmt->fmtp = strdup( "octet-align=1" );
            rtp_fmt->pf_packetize = rtp_packetize_amr;
            break;
        case VLC_CODEC_AMR_WB:
            rtp_fmt->ptname = "AMR-WB";
            rtp_fmt->fmtp = strdup( "octet-align=1" );
            rtp_fmt->pf_packetize = rtp_packetize_amr;
            break;
        case VLC_CODEC_SPEEX:
            rtp_fmt->ptname = "SPEEX";
            rtp_fmt->pf_packetize = rtp_packetize_spx;
            break;
        case VLC_CODEC_VORBIS:
            rtp_fmt->ptname = "vorbis";
            rtp_fmt->pf_packetize = rtp_packetize_xiph;
            if( p_fmt->i_extra > 0 )
            {
                rtp_fmt->fmtp = NULL;
                char *config = rtp_xiph_b64_oob_config(p_fmt->p_extra,
                                                       p_fmt->i_extra, NULL);
                if (config == NULL)
                    break;
                if( asprintf( &rtp_fmt->fmtp,
                              "configuration=%s;", config ) == -1 )
                    rtp_fmt->fmtp = NULL;
                free(config);
            }
            break;
        case VLC_CODEC_THEORA:
            rtp_fmt->ptname = "theora";
            rtp_fmt->pf_packetize = rtp_packetize_xiph;
            if( p_fmt->i_extra > 0 )
            {
                rtp_fmt->fmtp = NULL;
                uint8_t pixel_fmt, c1, c2;
                char *config = rtp_xiph_b64_oob_config(p_fmt->p_extra,
                                                       p_fmt->i_extra,
                                                       &pixel_fmt);
                if (config == NULL)
                    break;

                if (pixel_fmt == 1)
                {
                    /* reserved */
                    free(config);
                    break;
                }
                switch (pixel_fmt)
                {
                    case 0:
                        c1 = 2;
                        c2 = 0;
                        break;
                    case 2:
                        c1 = c2 = 2;
                        break;
                    case 3:
                        c1 = c2 = 4;
                        break;
                    default:
                        assert(0);
                }

                if( asprintf( &rtp_fmt->fmtp,
                              "sampling=YCbCr-4:%d:%d; width=%d; height=%d; "
                              "delivery-method=inline; configuration=%s; "
                              "delivery-method=in_band;", c1, c2,
                              p_fmt->video.i_width, p_fmt->video.i_height,
                              config ) == -1 )
                    rtp_fmt->fmtp = NULL;
                free(config);
            }
            break;
        case VLC_CODEC_ITU_T140:
            rtp_fmt->ptname = "t140" ;
            rtp_fmt->clock_rate = 1000;
            rtp_fmt->pf_packetize = rtp_packetize_t140;
            break;
        case VLC_CODEC_GSM:
            rtp_fmt->payload_type = 3;
            rtp_fmt->ptname = "GSM";
            rtp_fmt->pf_packetize = rtp_packetize_split;
            break;
        case VLC_CODEC_OPUS:
            if (p_fmt->audio.i_channels > 2)
            {
                msg_Err( obj, "Multistream opus not supported in RTP"
                         " (having %d channels input)",
                         p_fmt->audio.i_channels );
                return VLC_EGENERIC;
            }
            rtp_fmt->ptname = "opus";
            rtp_fmt->pf_packetize = rtp_packetize_simple;
            rtp_fmt->clock_rate = 48000;
            rtp_fmt->channels = 2;
            if (p_fmt->audio.i_channels == 2)
                rtp_fmt->fmtp = strdup( "sprop-stereo=1" );
            break;
        case VLC_CODEC_VP8:
            rtp_fmt->ptname = "VP8";
            rtp_fmt->pf_packetize = rtp_packetize_vp8;
            break;
        case VLC_CODEC_MJPG:
        case VLC_CODEC_JPEG:
            rtp_fmt->ptname = "JPEG";
            rtp_fmt->payload_type = 26;
            rtp_fmt->pf_packetize = rtp_packetize_jpeg;
            break;

        default:
            msg_Err( obj, "cannot add this stream (unsupported "
                     "codec: %4.4s)", (char*)&p_fmt->i_codec );
            return VLC_EGENERIC;
    }

    return VLC_SUCCESS;
}