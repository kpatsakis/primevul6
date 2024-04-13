static int MP4_ReadBox_stco_co64( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_co64_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_co64 );

    MP4_GET4BYTES( p_box->data.p_co64->i_entry_count );

    p_box->data.p_co64->i_chunk_offset =
        calloc( p_box->data.p_co64->i_entry_count, sizeof(uint64_t) );
    if( p_box->data.p_co64->i_chunk_offset == NULL )
        MP4_READBOX_EXIT( 0 );

    for( unsigned int i = 0; i < p_box->data.p_co64->i_entry_count; i++ )
    {
        if( p_box->i_type == ATOM_stco )
        {
            if( i_read < 4 )
            {
                break;
            }
            MP4_GET4BYTES( p_box->data.p_co64->i_chunk_offset[i] );
        }
        else
        {
            if( i_read < 8 )
            {
                break;
            }
            MP4_GET8BYTES( p_box->data.p_co64->i_chunk_offset[i] );
        }
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"co64\" entry-count %d",
                      p_box->data.p_co64->i_entry_count );

#endif
    MP4_READBOX_EXIT( 1 );
}