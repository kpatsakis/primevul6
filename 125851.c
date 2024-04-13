static int MP4_ReadBox_trun(  stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_trun_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_trun );

    MP4_GET4BYTES( p_box->data.p_trun->i_sample_count );

    if( p_box->data.p_trun->i_flags & MP4_TRUN_DATA_OFFSET )
        MP4_GET4BYTES( p_box->data.p_trun->i_data_offset );
    if( p_box->data.p_trun->i_flags & MP4_TRUN_FIRST_FLAGS )
        MP4_GET4BYTES( p_box->data.p_trun->i_first_sample_flags );

    p_box->data.p_trun->p_samples =
      calloc( p_box->data.p_trun->i_sample_count, sizeof(MP4_descriptor_trun_sample_t) );
    if ( p_box->data.p_trun->p_samples == NULL )
        MP4_READBOX_EXIT( 0 );

    for( unsigned int i = 0; i<p_box->data.p_trun->i_sample_count; i++ )
    {
        MP4_descriptor_trun_sample_t *p_sample = &p_box->data.p_trun->p_samples[i];
        if( p_box->data.p_trun->i_flags & MP4_TRUN_SAMPLE_DURATION )
            MP4_GET4BYTES( p_sample->i_duration );
        if( p_box->data.p_trun->i_flags & MP4_TRUN_SAMPLE_SIZE )
            MP4_GET4BYTES( p_sample->i_size );
        if( p_box->data.p_trun->i_flags & MP4_TRUN_SAMPLE_FLAGS )
            MP4_GET4BYTES( p_sample->i_flags );
        if( p_box->data.p_trun->i_flags & MP4_TRUN_SAMPLE_TIME_OFFSET )
            MP4_GET4BYTES( p_sample->i_composition_time_offset );
    }

#ifdef MP4_ULTRA_VERBOSE
    msg_Dbg( p_stream, "read box: \"trun\" version %u flags 0x%x sample count %u",
                  p_box->data.p_trun->i_version,
                  p_box->data.p_trun->i_flags,
                  p_box->data.p_trun->i_sample_count );

    for( unsigned int i = 0; i<p_box->data.p_trun->i_sample_count; i++ )
    {
        MP4_descriptor_trun_sample_t *p_sample = &p_box->data.p_trun->p_samples[i];
        msg_Dbg( p_stream, "read box: \"trun\" sample %4.4u flags 0x%x "\
            "duration %"PRIu32" size %"PRIu32" composition time offset %"PRIu32,
                        i, p_sample->i_flags, p_sample->i_duration,
                        p_sample->i_size, p_sample->i_composition_time_offset );
    }
#endif

    MP4_READBOX_EXIT( 1 );
}