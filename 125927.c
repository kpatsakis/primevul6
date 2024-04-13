static int MP4_ReadBox_elst( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_elst_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_elst );


    MP4_GET4BYTES( p_box->data.p_elst->i_entry_count );

    p_box->data.p_elst->i_segment_duration =
        calloc( p_box->data.p_elst->i_entry_count, sizeof(uint64_t) );
    p_box->data.p_elst->i_media_time =
        calloc( p_box->data.p_elst->i_entry_count, sizeof(int64_t) );
    p_box->data.p_elst->i_media_rate_integer =
        calloc( p_box->data.p_elst->i_entry_count, sizeof(uint16_t) );
    p_box->data.p_elst->i_media_rate_fraction =
        calloc( p_box->data.p_elst->i_entry_count, sizeof(uint16_t) );
    if( p_box->data.p_elst->i_segment_duration == NULL
     || p_box->data.p_elst->i_media_time == NULL
     || p_box->data.p_elst->i_media_rate_integer == NULL
     || p_box->data.p_elst->i_media_rate_fraction == NULL )
    {
        MP4_READBOX_EXIT( 0 );
    }

    unsigned i;
    for( i = 0; i < p_box->data.p_elst->i_entry_count; i++ )
    {
        if( p_box->data.p_elst->i_version == 1 )
        {
            if ( i_read < 20 )
                break;
            MP4_GET8BYTES( p_box->data.p_elst->i_segment_duration[i] );

            MP4_GET8BYTES( p_box->data.p_elst->i_media_time[i] );
        }
        else
        {
            if ( i_read < 12 )
                break;
            MP4_GET4BYTES( p_box->data.p_elst->i_segment_duration[i] );

            MP4_GET4BYTES( p_box->data.p_elst->i_media_time[i] );
            p_box->data.p_elst->i_media_time[i] = (int32_t)p_box->data.p_elst->i_media_time[i];
        }

        MP4_GET2BYTES( p_box->data.p_elst->i_media_rate_integer[i] );
        MP4_GET2BYTES( p_box->data.p_elst->i_media_rate_fraction[i] );
    }
    if ( i < p_box->data.p_elst->i_entry_count )
        p_box->data.p_elst->i_entry_count = i;
#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"elst\" entry-count %lu",
             (unsigned long)p_box->data.p_elst->i_entry_count );
#endif
    MP4_READBOX_EXIT( 1 );
}