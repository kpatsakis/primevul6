static int MP4_ReadBox_tref_generic( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_tref_generic_t );

    p_box->data.p_tref_generic->i_track_ID = NULL;
    p_box->data.p_tref_generic->i_entry_count = i_read / sizeof(uint32_t);
    if( p_box->data.p_tref_generic->i_entry_count > 0 )
        p_box->data.p_tref_generic->i_track_ID = calloc( p_box->data.p_tref_generic->i_entry_count, sizeof(uint32_t) );
    if( p_box->data.p_tref_generic->i_track_ID == NULL )
        MP4_READBOX_EXIT( 0 );

    for( unsigned i = 0; i < p_box->data.p_tref_generic->i_entry_count; i++ )
    {
        MP4_GET4BYTES( p_box->data.p_tref_generic->i_track_ID[i] );
    }
#ifdef MP4_VERBOSE
        msg_Dbg( p_stream, "read box: \"chap\" %d references",
                 p_box->data.p_tref_generic->i_entry_count );
#endif

    MP4_READBOX_EXIT( 1 );
}