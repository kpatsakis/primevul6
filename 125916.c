static void MP4_FreeBox_rdrf( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_rdrf->psz_ref );
}