static int MP4_ReadBox_padb( stream_t *p_stream, MP4_Box_t *p_box )
{
    uint32_t count;

    MP4_READBOX_ENTER( MP4_Box_data_padb_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_padb );

    MP4_GET4BYTES( p_box->data.p_padb->i_sample_count );
    count = (p_box->data.p_padb->i_sample_count + 1) / 2;

    p_box->data.p_padb->i_reserved1 = calloc( count, sizeof(uint16_t) );
    p_box->data.p_padb->i_pad2 = calloc( count, sizeof(uint16_t) );
    p_box->data.p_padb->i_reserved2 = calloc( count, sizeof(uint16_t) );
    p_box->data.p_padb->i_pad1 = calloc( count, sizeof(uint16_t) );
    if( p_box->data.p_padb->i_reserved1 == NULL
     || p_box->data.p_padb->i_pad2 == NULL
     || p_box->data.p_padb->i_reserved2 == NULL
     || p_box->data.p_padb->i_pad1 == NULL )
    {
        MP4_READBOX_EXIT( 0 );
    }

    for( unsigned int i = 0; i < i_read / 2 ; i++ )
    {
        if( i >= count )
        {
            MP4_READBOX_EXIT( 0 );
        }
        p_box->data.p_padb->i_reserved1[i] = ( (*p_peek) >> 7 )&0x01;
        p_box->data.p_padb->i_pad2[i] = ( (*p_peek) >> 4 )&0x07;
        p_box->data.p_padb->i_reserved1[i] = ( (*p_peek) >> 3 )&0x01;
        p_box->data.p_padb->i_pad1[i] = ( (*p_peek) )&0x07;

        p_peek += 1; i_read -= 1;
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"stdp\" entry-count %"PRId64,
                      i_read / 2 );

#endif
    MP4_READBOX_EXIT( 1 );
}