static void MP4_FreeBox_stdp( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_stdp->i_priority );
}