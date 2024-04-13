static void MP4_FreeBox_Binary( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_binary->p_blob );
    p_box->data.p_binary->i_blob = 0;
}