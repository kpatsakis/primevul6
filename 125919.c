static int MP4_ReadBox_tfxd(  stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_tfxd_t );

    MP4_Box_data_tfxd_t *p_tfxd_data = p_box->data.p_tfxd;
    MP4_GETVERSIONFLAGS( p_tfxd_data );

    if( p_tfxd_data->i_version == 0 )
    {
        MP4_GET4BYTES( p_tfxd_data->i_fragment_abs_time );
        MP4_GET4BYTES( p_tfxd_data->i_fragment_duration );
    }
    else
    {
        MP4_GET8BYTES( p_tfxd_data->i_fragment_abs_time );
        MP4_GET8BYTES( p_tfxd_data->i_fragment_duration );
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"tfxd\" version %d, flags 0x%x, "\
            "fragment duration %"PRIu64", fragment abs time %"PRIu64,
                p_tfxd_data->i_version,
                p_tfxd_data->i_flags,
                p_tfxd_data->i_fragment_duration,
                p_tfxd_data->i_fragment_abs_time
           );
#endif

    MP4_READBOX_EXIT( 1 );
}