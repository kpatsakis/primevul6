static int MP4_ReadBox_trex( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_trex_t );
    MP4_GETVERSIONFLAGS( p_box->data.p_trex );

    MP4_GET4BYTES( p_box->data.p_trex->i_track_ID );
    MP4_GET4BYTES( p_box->data.p_trex->i_default_sample_description_index );
    MP4_GET4BYTES( p_box->data.p_trex->i_default_sample_duration );
    MP4_GET4BYTES( p_box->data.p_trex->i_default_sample_size );
    MP4_GET4BYTES( p_box->data.p_trex->i_default_sample_flags );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"trex\" trackID: %"PRIu32"",
             p_box->data.p_trex->i_track_ID );
#endif

    MP4_READBOX_EXIT( 1 );
}