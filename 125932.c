static int MP4_ReadBox_data( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_data_t );
    MP4_Box_data_data_t *p_data = p_box->data.p_data;

    if ( i_read < 8 || i_read - 8 > UINT32_MAX )
        MP4_READBOX_EXIT( 0 );

    uint8_t i_type;
    MP4_GET1BYTE( i_type );
    if ( i_type != 0 )
    {
#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "skipping unknown 'data' atom with type %"PRIu8, i_type );
#endif
        MP4_READBOX_EXIT( 0 );
    }

    MP4_GET3BYTES( p_data->e_wellknowntype );
    MP4_GET2BYTES( p_data->locale.i_country );
    MP4_GET2BYTES( p_data->locale.i_language );
#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "read 'data' atom: knowntype=%"PRIu32", country=%"PRIu16" lang=%"PRIu16
                 ", size %"PRId64" bytes", p_data->e_wellknowntype,
                 p_data->locale.i_country, p_data->locale.i_language, i_read );
#endif
    p_box->data.p_data->p_blob = malloc( i_read );
    if ( !p_box->data.p_data->p_blob )
        MP4_READBOX_EXIT( 0 );

    p_box->data.p_data->i_blob = i_read;
    memcpy( p_box->data.p_data->p_blob, p_peek, i_read);

    MP4_READBOX_EXIT( 1 );
}