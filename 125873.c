static void MP4_FreeBox_stra( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_stra->CodecPrivateData );
}