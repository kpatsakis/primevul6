static void MP4_FreeBox_stsh( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_stsh->i_shadowed_sample_number );
    FREENULL( p_box->data.p_stsh->i_sync_sample_number );
}