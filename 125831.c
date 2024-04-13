static void MP4_FreeBox_sidx( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_sidx->p_items );
}