static void MP4_FreeBox_data( MP4_Box_t *p_box )
{
    free( p_box->data.p_data->p_blob );
}