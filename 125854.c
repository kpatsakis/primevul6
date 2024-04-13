static int MP4_ReadBox_rmqu( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_rmqu_t );

    MP4_GET4BYTES( p_box->data.p_rmqu->i_quality );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"rmqu\" quality:%d",
             p_box->data.p_rmqu->i_quality );
#endif
    MP4_READBOX_EXIT( 1 );
}