static int MP4_ReadBox_mfro( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_mfro_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_mfro );
    MP4_GET4BYTES( p_box->data.p_mfro->i_size );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"mfro\" size: %"PRIu32"",
             p_box->data.p_mfro->i_size);
#endif

    MP4_READBOX_EXIT( 1 );
}