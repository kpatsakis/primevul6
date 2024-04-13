void MP4_FreeBox_sample_vide( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_sample_vide->p_qt_image_description );
}