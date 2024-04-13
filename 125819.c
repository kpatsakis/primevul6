static int MP4_ReadBox_enda( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_Box_data_enda_t *p_enda;
    MP4_READBOX_ENTER( MP4_Box_data_enda_t );

    p_enda = p_box->data.p_enda;

    MP4_GET2BYTES( p_enda->i_little_endian );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"enda\" little_endian=%d", p_enda->i_little_endian );
#endif
    MP4_READBOX_EXIT( 1 );
}