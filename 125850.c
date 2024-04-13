static int MP4_ReadBox_mdhd( stream_t *p_stream, MP4_Box_t *p_box )
{
    uint16_t i_language;
#ifdef MP4_VERBOSE
    char s_creation_time[128];
    char s_modification_time[128];
    char s_duration[128];
#endif
    MP4_READBOX_ENTER( MP4_Box_data_mdhd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_mdhd );

    if( p_box->data.p_mdhd->i_version )
    {
        MP4_GET8BYTES( p_box->data.p_mdhd->i_creation_time );
        MP4_GET8BYTES( p_box->data.p_mdhd->i_modification_time );
        MP4_GET4BYTES( p_box->data.p_mdhd->i_timescale );
        MP4_GET8BYTES( p_box->data.p_mdhd->i_duration );
    }
    else
    {
        MP4_GET4BYTES( p_box->data.p_mdhd->i_creation_time );
        MP4_GET4BYTES( p_box->data.p_mdhd->i_modification_time );
        MP4_GET4BYTES( p_box->data.p_mdhd->i_timescale );
        MP4_GET4BYTES( p_box->data.p_mdhd->i_duration );
    }

    MP4_GET2BYTES( i_language );
    decodeQtLanguageCode( i_language, p_box->data.p_mdhd->rgs_language,
                          &p_box->data.p_mdhd->b_mac_encoding );

    MP4_GET2BYTES( p_box->data.p_mdhd->i_quality );

#ifdef MP4_VERBOSE
    MP4_ConvertDate2Str( s_creation_time, p_box->data.p_mdhd->i_creation_time, false );
    MP4_ConvertDate2Str( s_modification_time, p_box->data.p_mdhd->i_modification_time, false );
    MP4_ConvertDate2Str( s_duration, p_box->data.p_mdhd->i_duration, true );
    msg_Dbg( p_stream, "read box: \"mdhd\" creation %s modification %s time scale %d duration %s language %3.3s",
                  s_creation_time,
                  s_modification_time,
                  (uint32_t)p_box->data.p_mdhd->i_timescale,
                  s_duration,
                  (char*) &p_box->data.p_mdhd->rgs_language );
#endif
    MP4_READBOX_EXIT( 1 );
}