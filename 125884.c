static void MP4_FreeBox_url( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_url->psz_location );
}