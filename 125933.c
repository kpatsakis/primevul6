static void MP4_FreeBox_WMA2( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_WMA2->p_extra );
}