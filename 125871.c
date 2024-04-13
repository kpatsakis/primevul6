static void MP4_FreeBox_elst( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_elst->i_segment_duration );
    FREENULL( p_box->data.p_elst->i_media_time );
    FREENULL( p_box->data.p_elst->i_media_rate_integer );
    FREENULL( p_box->data.p_elst->i_media_rate_fraction );
}