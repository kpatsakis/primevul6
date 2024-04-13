static void MP4_FreeBox_hdlr( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_hdlr->psz_name );
}