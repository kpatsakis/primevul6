static int MP4_ReadBox_rmdr( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_rmdr_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_rmdr );

    MP4_GET4BYTES( p_box->data.p_rmdr->i_rate );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"rmdr\" rate:%d",
             p_box->data.p_rmdr->i_rate );
#endif
    MP4_READBOX_EXIT( 1 );
}