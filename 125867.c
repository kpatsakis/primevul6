static int MP4_ReadBox_ctts( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_ctts_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_ctts );

    MP4_GET4BYTES( p_box->data.p_ctts->i_entry_count );

    p_box->data.p_ctts->pi_sample_count =
        calloc( p_box->data.p_ctts->i_entry_count, sizeof(uint32_t) );
    p_box->data.p_ctts->pi_sample_offset =
        calloc( p_box->data.p_ctts->i_entry_count, sizeof(int32_t) );
    if( ( p_box->data.p_ctts->pi_sample_count == NULL )
     || ( p_box->data.p_ctts->pi_sample_offset == NULL ) )
    {
        MP4_READBOX_EXIT( 0 );
    }

    uint32_t i = 0;
    for( ; (i < p_box->data.p_ctts->i_entry_count )&&( i_read >=8 ); i++ )
    {
        MP4_GET4BYTES( p_box->data.p_ctts->pi_sample_count[i] );
        MP4_GET4BYTES( p_box->data.p_ctts->pi_sample_offset[i] );
    }
    if ( i < p_box->data.p_ctts->i_entry_count )
        p_box->data.p_ctts->i_entry_count = i;

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"ctts\" entry-count %d",
                      p_box->data.p_ctts->i_entry_count );

#endif
    MP4_READBOX_EXIT( 1 );
}