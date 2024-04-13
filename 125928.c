static void MP4_FreeBox_strf( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_strf->p_extra );
}