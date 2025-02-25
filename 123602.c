static block_t *Encode( encoder_t *p_enc, picture_t *p_pic )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    block_t *p_block, *p_output_chain = NULL;
    SchroFrame *p_frame;
    bool b_go = true;

    if( !p_pic ) {
        if( !p_sys->started || p_sys->b_eos_pulled )
            return NULL;

        if( !p_sys->b_eos_signalled ) {
            p_sys->b_eos_signalled = 1;
            schro_encoder_end_of_stream( p_sys->p_schro );
        }
    } else {
        /* we only know if the sequence is interlaced when the first
         * picture arrives, so final setup is done here */
        /* XXX todo, detect change of interlace */
        p_sys->p_format->interlaced = !p_pic->b_progressive;
        p_sys->p_format->top_field_first = p_pic->b_top_field_first;

        if( p_sys->b_auto_field_coding )
            schro_encoder_setting_set_double( p_sys->p_schro, "interlaced_coding", !p_pic->b_progressive );
    }

    if( !p_sys->started ) {
        date_t date;

        if( p_pic->format.i_chroma != p_enc->fmt_in.i_codec ) {
            char chroma_in[5], chroma_out[5];
            vlc_fourcc_to_char( p_pic->format.i_chroma, chroma_in );
            chroma_in[4]  = '\0';
            chroma_out[4] = '\0';
            vlc_fourcc_to_char( p_enc->fmt_in.i_codec, chroma_out );
            msg_Warn( p_enc, "Resetting chroma from %s to %s", chroma_out, chroma_in );
            if( !SetEncChromaFormat( p_enc, p_pic->format.i_chroma ) ) {
                msg_Err( p_enc, "Could not reset chroma format to %s", chroma_in );
                return NULL;
            }
        }

        date_Init( &date, p_enc->fmt_in.video.i_frame_rate, p_enc->fmt_in.video.i_frame_rate_base );
        /* FIXME - Unlike dirac-research codec Schro doesn't have a function that returns the delay in pics yet.
         *   Use a default of 1
         */
        date_Increment( &date, 1 );
        p_sys->i_pts_offset = date_Get( &date );
        if( schro_encoder_setting_get_double( p_sys->p_schro, "interlaced_coding" ) > 0.0 ) {
            date_Set( &date, 0 );
            date_Increment( &date, 1);
            p_sys->i_field_time = date_Get( &date ) / 2;
        }

        schro_video_format_set_std_signal_range( p_sys->p_format, SCHRO_SIGNAL_RANGE_8BIT_VIDEO );
        schro_encoder_set_video_format( p_sys->p_schro, p_sys->p_format );
        schro_encoder_start( p_sys->p_schro );
        p_sys->started = 1;
    }

    if( !p_sys->b_eos_signalled ) {
        /* create a schro frame from the input pic and load */
        /* Increase ref count by 1 so that the picture is not freed until
           Schro finishes with it */
        picture_Hold( p_pic );

        p_frame = CreateSchroFrameFromInputPic( p_enc, p_pic );
        if( !p_frame )
            return NULL;
        schro_encoder_push_frame( p_sys->p_schro, p_frame );


        /* store pts in a lookaside buffer, so that the same pts may
        * be used for the picture in coded order */
        StorePicturePTS( p_enc, p_sys->i_input_picnum, p_pic->date );
        p_sys->i_input_picnum++;

        /* store dts in a queue, so that they appear in order in
         * coded order */
        p_block = block_Alloc( 1 );
        if( !p_block )
            return NULL;
        p_block->i_dts = p_pic->date - p_sys->i_pts_offset;
        block_FifoPut( p_sys->p_dts_fifo, p_block );
        p_block = NULL;

        /* for field coding mode, insert an extra value into both the
         * pts lookaside buffer and dts queue, offset to correspond
         * to a one field delay. */
        if( schro_encoder_setting_get_double( p_sys->p_schro, "interlaced_coding" ) > 0.0 ) {
            StorePicturePTS( p_enc, p_sys->i_input_picnum, p_pic->date + p_sys->i_field_time );
            p_sys->i_input_picnum++;

            p_block = block_Alloc( 1 );
            if( !p_block )
                return NULL;
            p_block->i_dts = p_pic->date - p_sys->i_pts_offset + p_sys->i_field_time;
            block_FifoPut( p_sys->p_dts_fifo, p_block );
            p_block = NULL;
        }
    }

    do
    {
        SchroStateEnum state;
        state = schro_encoder_wait( p_sys->p_schro );
        switch( state )
        {
        case SCHRO_STATE_NEED_FRAME:
            b_go = false;
            break;
        case SCHRO_STATE_AGAIN:
            break;
        case SCHRO_STATE_END_OF_STREAM:
            p_sys->b_eos_pulled = 1;
            b_go = false;
            break;
        case SCHRO_STATE_HAVE_BUFFER:
        {
            SchroBuffer *p_enc_buf;
            uint32_t u_pic_num;
            int i_presentation_frame;
            p_enc_buf = schro_encoder_pull( p_sys->p_schro, &i_presentation_frame );
            p_block = block_Alloc( p_enc_buf->length );
            if( !p_block )
                return NULL;

            memcpy( p_block->p_buffer, p_enc_buf->data, p_enc_buf->length );
            schro_buffer_unref( p_enc_buf );

            /* Presence of a Sequence header indicates a seek point */
            if( 0 == p_block->p_buffer[4] )
            {
                p_block->i_flags |= BLOCK_FLAG_TYPE_I;

                if( !p_enc->fmt_out.p_extra ) {
                    const uint8_t eos[] = { 'B','B','C','D',0x10,0,0,0,13,0,0,0,0 };
                    uint32_t len = GetDWBE( p_block->p_buffer + 5 );
                    /* if it hasn't been done so far, stash a copy of the
                     * sequence header for muxers such as ogg */
                    /* The OggDirac spec advises that a Dirac EOS DataUnit
                     * is appended to the sequence header to allow guard
                     * against poor streaming servers */
                    /* XXX, should this be done using the packetizer ? */

                    if( len > UINT32_MAX - sizeof( eos ) )
                        return NULL;

                    p_enc->fmt_out.p_extra = malloc( len + sizeof( eos ) );
                    if( !p_enc->fmt_out.p_extra )
                        return NULL;
                    memcpy( p_enc->fmt_out.p_extra, p_block->p_buffer, len );
                    memcpy( (uint8_t*)p_enc->fmt_out.p_extra + len, eos, sizeof( eos ) );
                    SetDWBE( (uint8_t*)p_enc->fmt_out.p_extra + len + sizeof(eos) - 4, len );
                    p_enc->fmt_out.i_extra = len + sizeof( eos );
                }
            }

            if( ReadDiracPictureNumber( &u_pic_num, p_block ) ) {
                block_t *p_dts_block = block_FifoGet( p_sys->p_dts_fifo );
                p_block->i_dts = p_dts_block->i_dts;
                   p_block->i_pts = GetPicturePTS( p_enc, u_pic_num );
                block_Release( p_dts_block );
                block_ChainAppend( &p_output_chain, p_block );
            } else {
                /* End of sequence */
                block_ChainAppend( &p_output_chain, p_block );
            }
            break;
        }
        default:
            break;
        }
    } while( b_go );

    return p_output_chain;
}