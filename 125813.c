static void MP4_FreeBox_stsz( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_stsz->i_entry_size );
}