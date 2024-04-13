static void MP4_FreeBox_sample_text( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_sample_text->psz_text_name );
}