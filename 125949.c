static int MP4_ReadBox_sample_soun( stream_t *p_stream, MP4_Box_t *p_box )
{
    p_box->i_handler = ATOM_soun;
    MP4_READBOX_ENTER( MP4_Box_data_sample_soun_t );
    p_box->data.p_sample_soun->p_qt_description = NULL;

    /* Sanity check needed because the "wave" box does also contain an
     * "mp4a" box that we don't understand. */
    if( i_read < 28 )
    {
        i_read -= 30;
        MP4_READBOX_EXIT( 1 );
    }

    for( unsigned i = 0; i < 6 ; i++ )
    {
        MP4_GET1BYTE( p_box->data.p_sample_soun->i_reserved1[i] );
    }

    MP4_GET2BYTES( p_box->data.p_sample_soun->i_data_reference_index );

    /*
     * XXX hack -> produce a copy of the nearly complete chunk
     */
    p_box->data.p_sample_soun->i_qt_description = 0;
    p_box->data.p_sample_soun->p_qt_description = NULL;
    if( i_read > 0 )
    {
        p_box->data.p_sample_soun->p_qt_description = malloc( i_read );
        if( p_box->data.p_sample_soun->p_qt_description )
        {
            p_box->data.p_sample_soun->i_qt_description = i_read;
            memcpy( p_box->data.p_sample_soun->p_qt_description, p_peek, i_read );
        }
    }

    MP4_GET2BYTES( p_box->data.p_sample_soun->i_qt_version );
    MP4_GET2BYTES( p_box->data.p_sample_soun->i_qt_revision_level );
    MP4_GET4BYTES( p_box->data.p_sample_soun->i_qt_vendor );

    MP4_GET2BYTES( p_box->data.p_sample_soun->i_channelcount );
    MP4_GET2BYTES( p_box->data.p_sample_soun->i_samplesize );
    MP4_GET2BYTES( p_box->data.p_sample_soun->i_compressionid );
    MP4_GET2BYTES( p_box->data.p_sample_soun->i_reserved3 );
    MP4_GET2BYTES( p_box->data.p_sample_soun->i_sampleratehi );
    MP4_GET2BYTES( p_box->data.p_sample_soun->i_sampleratelo );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"soun\" stsd qt_version %"PRIu16" compid=%"PRIx16,
             p_box->data.p_sample_soun->i_qt_version,
             p_box->data.p_sample_soun->i_compressionid );
#endif

    if( p_box->data.p_sample_soun->i_qt_version == 1 && i_read >= 16 )
    {
        /* SoundDescriptionV1 */
        MP4_GET4BYTES( p_box->data.p_sample_soun->i_sample_per_packet );
        MP4_GET4BYTES( p_box->data.p_sample_soun->i_bytes_per_packet );
        MP4_GET4BYTES( p_box->data.p_sample_soun->i_bytes_per_frame );
        MP4_GET4BYTES( p_box->data.p_sample_soun->i_bytes_per_sample );

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream,
                 "read box: \"soun\" V1 sample/packet=%d bytes/packet=%d "
                 "bytes/frame=%d bytes/sample=%d",
                 p_box->data.p_sample_soun->i_sample_per_packet,
                 p_box->data.p_sample_soun->i_bytes_per_packet,
                 p_box->data.p_sample_soun->i_bytes_per_frame,
                 p_box->data.p_sample_soun->i_bytes_per_sample );
#endif
        stream_Seek( p_stream, p_box->i_pos +
                        mp4_box_headersize( p_box ) + 44 );
    }
    else if( p_box->data.p_sample_soun->i_qt_version == 2 && i_read >= 36 )
    {
        /* SoundDescriptionV2 */
        double f_sample_rate;
        int64_t i_dummy64;
        uint32_t i_channel, i_extoffset, i_dummy32;

        /* Checks */
        if ( p_box->data.p_sample_soun->i_channelcount != 0x3  ||
             p_box->data.p_sample_soun->i_samplesize != 0x0010 ||
             p_box->data.p_sample_soun->i_compressionid != 0xFFFE ||
             p_box->data.p_sample_soun->i_reserved3 != 0x0     ||
             p_box->data.p_sample_soun->i_sampleratehi != 0x1  ||//65536
             p_box->data.p_sample_soun->i_sampleratelo != 0x0 )  //remainder
        {
            msg_Err( p_stream, "invalid stsd V2 box defaults" );
            MP4_READBOX_EXIT( 0 );
        }
        /* !Checks */

        MP4_GET4BYTES( i_extoffset ); /* offset to stsd extentions */
        MP4_GET8BYTES( i_dummy64 );
        memcpy( &f_sample_rate, &i_dummy64, 8 );
        msg_Dbg( p_stream, "read box: %f Hz", f_sample_rate );
        p_box->data.p_sample_soun->i_sampleratehi = (int)f_sample_rate % BLOCK16x16;
        p_box->data.p_sample_soun->i_sampleratelo = f_sample_rate / BLOCK16x16;

        MP4_GET4BYTES( i_channel );
        p_box->data.p_sample_soun->i_channelcount = i_channel;

        MP4_GET4BYTES( i_dummy32 );
        if ( i_dummy32 != 0x7F000000 )
        {
            msg_Err( p_stream, "invalid stsd V2 box" );
            MP4_READBOX_EXIT( 0 );
        }

        MP4_GET4BYTES( p_box->data.p_sample_soun->i_constbitsperchannel );
        MP4_GET4BYTES( p_box->data.p_sample_soun->i_formatflags );
        MP4_GET4BYTES( p_box->data.p_sample_soun->i_constbytesperaudiopacket );
        MP4_GET4BYTES( p_box->data.p_sample_soun->i_constLPCMframesperaudiopacket );

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "read box: \"soun\" V2 rate=%f bitsperchannel=%u "
                           "flags=%u bytesperpacket=%u lpcmframesperpacket=%u",
                 f_sample_rate,
                 p_box->data.p_sample_soun->i_constbitsperchannel,
                 p_box->data.p_sample_soun->i_formatflags,
                 p_box->data.p_sample_soun->i_constbytesperaudiopacket,
                 p_box->data.p_sample_soun->i_constLPCMframesperaudiopacket );
#endif
        if ( i_extoffset < p_box->i_size )
            stream_Seek( p_stream, p_box->i_pos + i_extoffset );
        else
            stream_Seek( p_stream, p_box->i_pos + p_box->i_size );
    }
    else
    {
        p_box->data.p_sample_soun->i_sample_per_packet = 0;
        p_box->data.p_sample_soun->i_bytes_per_packet = 0;
        p_box->data.p_sample_soun->i_bytes_per_frame = 0;
        p_box->data.p_sample_soun->i_bytes_per_sample = 0;

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "read box: \"soun\" V0 or qt1/2 (rest=%"PRId64")",
                 i_read );
#endif
        stream_Seek( p_stream, p_box->i_pos +
                        mp4_box_headersize( p_box ) + 28 );
    }

    if( p_box->i_type == ATOM_drms )
    {
        msg_Warn( p_stream, "DRM protected streams are not supported." );
        MP4_READBOX_EXIT( 0 );
    }

    if( p_box->i_type == ATOM_samr || p_box->i_type == ATOM_sawb )
    {
        /* Ignore channelcount for AMR (3gpp AMRSpecificBox) */
        p_box->data.p_sample_soun->i_channelcount = 1;
    }

    /* Loads extensions */
    MP4_ReadBoxContainerRaw( p_stream, p_box ); /* esds/wave/... */

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"soun\" in stsd channel %d "
             "sample size %d sample rate %f",
             p_box->data.p_sample_soun->i_channelcount,
             p_box->data.p_sample_soun->i_samplesize,
             (float)p_box->data.p_sample_soun->i_sampleratehi +
             (float)p_box->data.p_sample_soun->i_sampleratelo / BLOCK16x16 );

#endif
    MP4_READBOX_EXIT( 1 );
}