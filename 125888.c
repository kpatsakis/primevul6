static int MP4_ReadBox_mehd( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_mehd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_mehd );
    if( p_box->data.p_mehd->i_version == 1 )
        MP4_GET8BYTES( p_box->data.p_mehd->i_fragment_duration );
    else /* version == 0 */
        MP4_GET4BYTES( p_box->data.p_mehd->i_fragment_duration );

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"mehd\" frag dur. %"PRIu64"",
             p_box->data.p_mehd->i_fragment_duration );
#endif

    MP4_READBOX_EXIT( 1 );
}