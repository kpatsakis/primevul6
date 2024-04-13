static void MP4_FreeBox_sdtp( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_sdtp->p_sample_table );
}