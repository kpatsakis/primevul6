static int MP4_ReadBox_mvhd(  stream_t *p_stream, MP4_Box_t *p_box )
{
#ifdef MP4_VERBOSE
    char s_creation_time[128];
    char s_modification_time[128];
    char s_duration[128];
#endif
    MP4_READBOX_ENTER( MP4_Box_data_mvhd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_mvhd );

    if( p_box->data.p_mvhd->i_version )
    {
        MP4_GET8BYTES( p_box->data.p_mvhd->i_creation_time );
        MP4_GET8BYTES( p_box->data.p_mvhd->i_modification_time );
        MP4_GET4BYTES( p_box->data.p_mvhd->i_timescale );
        MP4_GET8BYTES( p_box->data.p_mvhd->i_duration );
    }
    else
    {
        MP4_GET4BYTES( p_box->data.p_mvhd->i_creation_time );
        MP4_GET4BYTES( p_box->data.p_mvhd->i_modification_time );
        MP4_GET4BYTES( p_box->data.p_mvhd->i_timescale );
        MP4_GET4BYTES( p_box->data.p_mvhd->i_duration );
    }
    MP4_GET4BYTES( p_box->data.p_mvhd->i_rate );
    MP4_GET2BYTES( p_box->data.p_mvhd->i_volume );
    MP4_GET2BYTES( p_box->data.p_mvhd->i_reserved1 );


    for( unsigned i = 0; i < 2; i++ )
    {
        MP4_GET4BYTES( p_box->data.p_mvhd->i_reserved2[i] );
    }
    for( unsigned i = 0; i < 9; i++ )
    {
        MP4_GET4BYTES( p_box->data.p_mvhd->i_matrix[i] );
    }
    for( unsigned i = 0; i < 6; i++ )
    {
        MP4_GET4BYTES( p_box->data.p_mvhd->i_predefined[i] );
    }

    MP4_GET4BYTES( p_box->data.p_mvhd->i_next_track_id );


#ifdef MP4_VERBOSE
    MP4_ConvertDate2Str( s_creation_time, p_box->data.p_mvhd->i_creation_time, false );
    MP4_ConvertDate2Str( s_modification_time,
                         p_box->data.p_mvhd->i_modification_time, false );
    if( p_box->data.p_mvhd->i_rate )
    {
        MP4_ConvertDate2Str( s_duration,
                 p_box->data.p_mvhd->i_duration / p_box->data.p_mvhd->i_rate, true );
    }
    else
    {
        s_duration[0] = 0;
    }
    msg_Dbg( p_stream, "read box: \"mvhd\" creation %s modification %s time scale %d duration %s rate %f volume %f next track id %d",
                  s_creation_time,
                  s_modification_time,
                  (uint32_t)p_box->data.p_mvhd->i_timescale,
                  s_duration,
                  (float)p_box->data.p_mvhd->i_rate / (1<<16 ),
                  (float)p_box->data.p_mvhd->i_volume / 256 ,
                  (uint32_t)p_box->data.p_mvhd->i_next_track_id );
#endif
    MP4_READBOX_EXIT( 1 );
}