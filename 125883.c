static int MP4_ReadBox_mfhd(  stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_mfhd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_mvhd );

    MP4_GET4BYTES( p_box->data.p_mfhd->i_sequence_number );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"mfhd\" sequence number %d",
                  p_box->data.p_mfhd->i_sequence_number );
#endif
    MP4_READBOX_EXIT( 1 );
}