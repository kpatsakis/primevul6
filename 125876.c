static void MP4_FreeBox_trun( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_trun->p_samples );
}