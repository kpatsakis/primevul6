static int MP4_ReadBox_load( stream_t *p_stream, MP4_Box_t *p_box )
{
    if ( p_box->i_size != 24 )
        return 0;
    MP4_READBOX_ENTER( MP4_Box_data_load_t );
    MP4_GET4BYTES( p_box->data.p_load->i_start_time );
    MP4_GET4BYTES( p_box->data.p_load->i_duration );
    MP4_GET4BYTES( p_box->data.p_load->i_flags );
    MP4_GET4BYTES( p_box->data.p_load->i_hints );
    MP4_READBOX_EXIT( 1 );
}