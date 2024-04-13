static int MP4_ReadBox_tfrf(  stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_tfxd_t );

    MP4_Box_data_tfrf_t *p_tfrf_data = p_box->data.p_tfrf;
    MP4_GETVERSIONFLAGS( p_tfrf_data );

    MP4_GET1BYTE( p_tfrf_data->i_fragment_count );

    p_tfrf_data->p_tfrf_data_fields = calloc( p_tfrf_data->i_fragment_count,
                                              sizeof( TfrfBoxDataFields_t ) );
    if( !p_tfrf_data->p_tfrf_data_fields )
        MP4_READBOX_EXIT( 0 );

    for( uint8_t i = 0; i < p_tfrf_data->i_fragment_count; i++ )
    {
        TfrfBoxDataFields_t *TfrfBoxDataField = &p_tfrf_data->p_tfrf_data_fields[i];
        if( p_tfrf_data->i_version == 0 )
        {
            MP4_GET4BYTES( TfrfBoxDataField->i_fragment_abs_time );
            MP4_GET4BYTES( TfrfBoxDataField->i_fragment_duration );
        }
        else
        {
            MP4_GET8BYTES( TfrfBoxDataField->i_fragment_abs_time );
            MP4_GET8BYTES( TfrfBoxDataField->i_fragment_duration );
        }
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"tfrf\" version %d, flags 0x%x, "\
            "fragment count %"PRIu8, p_tfrf_data->i_version,
                p_tfrf_data->i_flags, p_tfrf_data->i_fragment_count );

    for( uint8_t i = 0; i < p_tfrf_data->i_fragment_count; i++ )
    {
        TfrfBoxDataFields_t *TfrfBoxDataField = &p_tfrf_data->p_tfrf_data_fields[i];
        msg_Dbg( p_stream, "\"tfrf\" fragment duration %"PRIu64", "\
                                    "fragment abs time %"PRIu64,
                    TfrfBoxDataField->i_fragment_duration,
                    TfrfBoxDataField->i_fragment_abs_time );
    }

#endif

    MP4_READBOX_EXIT( 1 );
}