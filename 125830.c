static int MP4_ReadBox_stsz( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_stsz_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_stsz );

    MP4_GET4BYTES( p_box->data.p_stsz->i_sample_size );
    MP4_GET4BYTES( p_box->data.p_stsz->i_sample_count );

    if( p_box->data.p_stsz->i_sample_size == 0 )
    {
        p_box->data.p_stsz->i_entry_size =
            calloc( p_box->data.p_stsz->i_sample_count, sizeof(uint32_t) );
        if( unlikely( !p_box->data.p_stsz->i_entry_size ) )
            MP4_READBOX_EXIT( 0 );

        for( unsigned int i = 0; (i<p_box->data.p_stsz->i_sample_count)&&(i_read >= 4 ); i++ )
        {
            MP4_GET4BYTES( p_box->data.p_stsz->i_entry_size[i] );
        }
    }
    else
        p_box->data.p_stsz->i_entry_size = NULL;

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"stsz\" sample-size %d sample-count %d",
                      p_box->data.p_stsz->i_sample_size,
                      p_box->data.p_stsz->i_sample_count );

#endif
    MP4_READBOX_EXIT( 1 );
}