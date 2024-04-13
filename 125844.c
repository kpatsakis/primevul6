static int MP4_ReadBox_stsh( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_stsh_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_stsh );


    MP4_GET4BYTES( p_box->data.p_stsh->i_entry_count );

    p_box->data.p_stsh->i_shadowed_sample_number =
        calloc( p_box->data.p_stsh->i_entry_count, sizeof(uint32_t) );
    p_box->data.p_stsh->i_sync_sample_number =
        calloc( p_box->data.p_stsh->i_entry_count, sizeof(uint32_t) );

    if( p_box->data.p_stsh->i_shadowed_sample_number == NULL
     || p_box->data.p_stsh->i_sync_sample_number == NULL )
    {
        MP4_READBOX_EXIT( 0 );
    }

    unsigned i;
    for( i = 0; (i < p_box->data.p_stss->i_entry_count )&&( i_read >= 8 ); i++ )
    {
        MP4_GET4BYTES( p_box->data.p_stsh->i_shadowed_sample_number[i] );
        MP4_GET4BYTES( p_box->data.p_stsh->i_sync_sample_number[i] );
    }
    if ( i < p_box->data.p_stss->i_entry_count )
        p_box->data.p_stss->i_entry_count = i;

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"stsh\" entry-count %d",
                      p_box->data.p_stsh->i_entry_count );
#endif
    MP4_READBOX_EXIT( 1 );
}