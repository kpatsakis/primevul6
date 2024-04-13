static int MP4_ReadBox_hdlr( stream_t *p_stream, MP4_Box_t *p_box )
{
    int32_t i_reserved;
    VLC_UNUSED(i_reserved);

    MP4_READBOX_ENTER( MP4_Box_data_hdlr_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_hdlr );

    MP4_GETFOURCC( p_box->data.p_hdlr->i_predefined );
    MP4_GETFOURCC( p_box->data.p_hdlr->i_handler_type );

    MP4_GET4BYTES( i_reserved );
    MP4_GET4BYTES( i_reserved );
    MP4_GET4BYTES( i_reserved );
    p_box->data.p_hdlr->psz_name = NULL;

    if( i_read > 0 )
    {
        uint8_t *psz = p_box->data.p_hdlr->psz_name = malloc( i_read + 1 );
        if( unlikely( psz == NULL ) )
            MP4_READBOX_EXIT( 0 );

        /* Yes, I love .mp4 :( */
        if( p_box->data.p_hdlr->i_predefined == VLC_FOURCC( 'm', 'h', 'l', 'r' ) )
        {
            uint8_t i_len;
            int i_copy;

            MP4_GET1BYTE( i_len );
            i_copy = __MIN( i_read, i_len );

            memcpy( psz, p_peek, i_copy );
            p_box->data.p_hdlr->psz_name[i_copy] = '\0';
        }
        else
        {
            memcpy( psz, p_peek, i_read );
            p_box->data.p_hdlr->psz_name[i_read] = '\0';
        }
    }

#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "read box: \"hdlr\" handler type: \"%4.4s\" name: \"%s\"",
                   (char*)&p_box->data.p_hdlr->i_handler_type,
                   p_box->data.p_hdlr->psz_name );

#endif
    MP4_READBOX_EXIT( 1 );
}