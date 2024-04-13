static int MP4_ReadBoxSkip( stream_t *p_stream, MP4_Box_t *p_box )
{
    /* XXX sometime moov is hiden in a free box */
    if( p_box->p_father &&
        p_box->p_father->i_type == ATOM_root &&
        p_box->i_type == ATOM_free )
    {
        const uint8_t *p_peek;
        int     i_read;
        vlc_fourcc_t i_fcc;

        i_read  = stream_Peek( p_stream, &p_peek, 44 );

        p_peek += mp4_box_headersize( p_box ) + 4;
        i_read -= mp4_box_headersize( p_box ) + 4;

        if( i_read >= 8 )
        {
            i_fcc = VLC_FOURCC( p_peek[0], p_peek[1], p_peek[2], p_peek[3] );

            if( i_fcc == ATOM_cmov || i_fcc == ATOM_mvhd )
            {
                msg_Warn( p_stream, "detected moov hidden in a free box ..." );

                p_box->i_type = ATOM_foov;
                return MP4_ReadBoxContainer( p_stream, p_box );
            }
        }
    }

    /* Nothing to do */
#ifdef MP4_ULTRA_VERBOSE
    if MP4_BOX_TYPE_ASCII()
        msg_Dbg( p_stream, "skip box: \"%4.4s\"", (char*)&p_box->i_type );
    else
        msg_Dbg( p_stream, "skip box: \"c%3.3s\"", (char*)&p_box->i_type+1 );
#endif
    return 1;
}