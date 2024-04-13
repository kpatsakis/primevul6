static int MP4_ReadBox_tfhd(  stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_tfhd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_tfhd );

    if( p_box->data.p_tfhd->i_version != 0 )
    {
        msg_Warn( p_stream, "'tfhd' box with version != 0. "\
                " Don't know what to do with that, please patch" );
        MP4_READBOX_EXIT( 0 );
    }

    MP4_GET4BYTES( p_box->data.p_tfhd->i_track_ID );

    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_DURATION_IS_EMPTY )
    {
        msg_Dbg( p_stream, "'duration-is-empty' flag is present "\
                "=> no samples for this time interval." );
        p_box->data.p_tfhd->b_empty = true;
    }
    else
        p_box->data.p_tfhd->b_empty = false;

    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_BASE_DATA_OFFSET )
        MP4_GET8BYTES( p_box->data.p_tfhd->i_base_data_offset );
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_SAMPLE_DESC_INDEX )
        MP4_GET4BYTES( p_box->data.p_tfhd->i_sample_description_index );
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_DFLT_SAMPLE_DURATION )
        MP4_GET4BYTES( p_box->data.p_tfhd->i_default_sample_duration );
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_DFLT_SAMPLE_SIZE )
        MP4_GET4BYTES( p_box->data.p_tfhd->i_default_sample_size );
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_DFLT_SAMPLE_FLAGS )
        MP4_GET4BYTES( p_box->data.p_tfhd->i_default_sample_flags );

#ifdef MP4_VERBOSE
    char psz_base[128] = "\0";
    char psz_desc[128] = "\0";
    char psz_dura[128] = "\0";
    char psz_size[128] = "\0";
    char psz_flag[128] = "\0";
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_BASE_DATA_OFFSET )
        snprintf(psz_base, sizeof(psz_base), "base offset %"PRId64, p_box->data.p_tfhd->i_base_data_offset);
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_SAMPLE_DESC_INDEX )
        snprintf(psz_desc, sizeof(psz_desc), "sample description index %d", p_box->data.p_tfhd->i_sample_description_index);
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_DFLT_SAMPLE_DURATION )
        snprintf(psz_dura, sizeof(psz_dura), "sample duration %d", p_box->data.p_tfhd->i_default_sample_duration);
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_DFLT_SAMPLE_SIZE )
        snprintf(psz_size, sizeof(psz_size), "sample size %d", p_box->data.p_tfhd->i_default_sample_size);
    if( p_box->data.p_tfhd->i_flags & MP4_TFHD_DFLT_SAMPLE_FLAGS )
        snprintf(psz_flag, sizeof(psz_flag), "sample flags 0x%x", p_box->data.p_tfhd->i_default_sample_flags);

    msg_Dbg( p_stream, "read box: \"tfhd\" version %d flags 0x%x track ID %d %s %s %s %s %s",
                p_box->data.p_tfhd->i_version,
                p_box->data.p_tfhd->i_flags,
                p_box->data.p_tfhd->i_track_ID,
                psz_base, psz_desc, psz_dura, psz_size, psz_flag );
#endif

    MP4_READBOX_EXIT( 1 );
}