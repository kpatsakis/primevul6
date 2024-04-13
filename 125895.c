static void MP4_FreeBox_stts( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_stts->pi_sample_count );
    FREENULL( p_box->data.p_stts->pi_sample_delta );
}