static int MP4_ReadBox_stsc( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_stsc_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_stsc );

    MP4_GET4BYTES( p_box->data.p_stsc->i_entry_count );

    p_box->data.p_stsc->i_first_chunk =
        calloc( p_box->data.p_stsc->i_entry_count, sizeof(uint32_t) );
    p_box->data.p_stsc->i_samples_per_chunk =
        calloc( p_box->data.p_stsc->i_entry_count, sizeof(uint32_t) );
    p_box->data.p_stsc->i_sample_description_index =
        calloc( p_box->data.p_stsc->i_entry_count, sizeof(uint32_t) );
    if( unlikely( p_box->data.p_stsc->i_first_chunk == NULL
     || p_box->data.p_stsc->i_samples_per_chunk == NULL
     || p_box->data.p_stsc->i_sample_description_index == NULL ) )
    {
        MP4_READBOX_EXIT( 0 );
    }

    for( unsigned int i = 0; (i < p_box->data.p_stsc->i_entry_count )&&( i_read >= 12 );i++ )
    {
        MP4_GET4BYTES( p_box->data.p_stsc->i_first_chunk[i] );
        MP4_GET4BYTES( p_box->data.p_stsc->i_samples_per_chunk[i] );
        MP4_GET4BYTES( p_box->data.p_stsc->i_sample_description_index[i] );
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"stsc\" entry-count %d",
                      p_box->data.p_stsc->i_entry_count );

#endif
    MP4_READBOX_EXIT( 1 );
}