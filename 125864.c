static void MP4_FreeBox_ctts( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_ctts->pi_sample_count );
    FREENULL( p_box->data.p_ctts->pi_sample_offset );
}