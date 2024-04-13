int MP4_ReadBoxCommon( stream_t *p_stream, MP4_Box_t *p_box )
{
    int      i_read;
    const uint8_t  *p_peek;

    if( ( ( i_read = stream_Peek( p_stream, &p_peek, 32 ) ) < 8 ) )
    {
        return 0;
    }
    p_box->i_pos = stream_Tell( p_stream );

    p_box->data.p_payload = NULL;
    p_box->p_father = NULL;
    p_box->p_first  = NULL;
    p_box->p_last  = NULL;
    p_box->p_next   = NULL;

    MP4_GET4BYTES( p_box->i_shortsize );
    MP4_GETFOURCC( p_box->i_type );

    /* Now special case */

    if( p_box->i_shortsize == 1 )
    {
        /* get the true size on 64 bits */
        MP4_GET8BYTES( p_box->i_size );
    }
    else
    {
        p_box->i_size = p_box->i_shortsize;
        /* XXX size of 0 means that the box extends to end of file */
    }

    if( p_box->i_type == ATOM_uuid )
    {
        /* get extented type on 16 bytes */
        GetUUID( &p_box->i_uuid, p_peek );
        p_peek += 16; i_read -= 16;
    }
    else
    {
        CreateUUID( &p_box->i_uuid, p_box->i_type );
    }
#ifdef MP4_ULTRA_VERBOSE
    if( p_box->i_size )
    {
        if MP4_BOX_TYPE_ASCII()
            msg_Dbg( p_stream, "found Box: %4.4s size %"PRId64" %"PRId64,
                    (char*)&p_box->i_type, p_box->i_size, p_box->i_pos );
        else
            msg_Dbg( p_stream, "found Box: c%3.3s size %"PRId64,
                    (char*)&p_box->i_type+1, p_box->i_size );
    }
#endif

    return 1;
}