static void MP4_FreeBox_stsc( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_stsc->i_first_chunk );
    FREENULL( p_box->data.p_stsc->i_samples_per_chunk );
    FREENULL( p_box->data.p_stsc->i_sample_description_index );
}