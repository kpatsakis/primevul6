static int MP4_ReadBox_sdtp( stream_t *p_stream, MP4_Box_t *p_box )
{
    uint32_t i_sample_count;
    MP4_READBOX_ENTER( MP4_Box_data_sdtp_t );
    MP4_Box_data_sdtp_t *p_sdtp = p_box->data.p_sdtp;
    MP4_GETVERSIONFLAGS( p_box->data.p_sdtp );
    i_sample_count = i_read;

    p_sdtp->p_sample_table = calloc( i_sample_count, 1 );

    if( !p_sdtp->p_sample_table )
        MP4_READBOX_EXIT( 0 );

    for( uint32_t i = 0; i < i_sample_count; i++ )
        MP4_GET1BYTE( p_sdtp->p_sample_table[i] );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "i_sample_count is %"PRIu32"", i_sample_count );
    if ( i_sample_count > 3 )
        msg_Dbg( p_stream,
             "read box: \"sdtp\" head: %"PRIx8" %"PRIx8" %"PRIx8" %"PRIx8"",
                 p_sdtp->p_sample_table[0],
                 p_sdtp->p_sample_table[1],
                 p_sdtp->p_sample_table[2],
                 p_sdtp->p_sample_table[3] );
#endif

    MP4_READBOX_EXIT( 1 );
}