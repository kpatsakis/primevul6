static void MP4_FreeBox_cmvd( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_cmvd->p_data );
}