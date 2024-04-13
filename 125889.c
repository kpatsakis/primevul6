static void MP4_FreeBox_String( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_string->psz_text );
}