static int MP4_ReadBox_sidx(  stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_sidx_t );

    MP4_Box_data_sidx_t *p_sidx_data = p_box->data.p_sidx;
    MP4_GETVERSIONFLAGS( p_sidx_data );

    MP4_GET4BYTES( p_sidx_data->i_reference_ID );
    MP4_GET4BYTES( p_sidx_data->i_timescale );

    if( p_sidx_data->i_version == 0 )
    {
        MP4_GET4BYTES( p_sidx_data->i_earliest_presentation_time );
        MP4_GET4BYTES( p_sidx_data->i_first_offset );
    }
    else
    {
        MP4_GET8BYTES( p_sidx_data->i_earliest_presentation_time );
        MP4_GET8BYTES( p_sidx_data->i_first_offset );
    }

    uint16_t i_reserved;
    VLC_UNUSED(i_reserved);
    MP4_GET2BYTES( i_reserved );
    MP4_GET2BYTES( p_sidx_data->i_reference_count );
    uint16_t i_count = p_sidx_data->i_reference_count;

    p_sidx_data->p_items = calloc( i_count, sizeof( MP4_Box_sidx_item_t ) );
    uint32_t tmp;
    for( unsigned i = 0; i < i_count; i++ )
    {
        MP4_GET4BYTES( tmp );
        p_sidx_data->p_items[i].b_reference_type = (bool)((tmp & 0x80000000)>>24);
        p_sidx_data->p_items[i].i_referenced_size = tmp & 0x7fffffff;
        MP4_GET4BYTES( p_sidx_data->p_items[i].i_subsegment_duration );

        MP4_GET4BYTES( tmp );
        p_sidx_data->p_items[i].b_starts_with_SAP = (bool)((tmp & 0x80000000)>>24);
        p_sidx_data->p_items[i].i_SAP_type = (tmp & 0x70000000)>>24;
        p_sidx_data->p_items[i].i_SAP_delta_time = tmp & 0xfffffff;
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"sidx\" version %d, flags 0x%x, "\
            "ref_ID %"PRIu32", timescale %"PRIu32", ref_count %"PRIu16", "\
            "first subsegmt duration %"PRIu32,
                p_sidx_data->i_version,
                p_sidx_data->i_flags,
                p_sidx_data->i_reference_ID,
                p_sidx_data->i_timescale,
                p_sidx_data->i_reference_count,
                p_sidx_data->p_items[0].i_subsegment_duration
           );
#endif

    MP4_READBOX_EXIT( 1 );
}