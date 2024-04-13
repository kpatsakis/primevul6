static void MP4_FreeBox_tfra( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_tfra->p_time );
    FREENULL( p_box->data.p_tfra->p_moof_offset );
    FREENULL( p_box->data.p_tfra->p_traf_number );
    FREENULL( p_box->data.p_tfra->p_trun_number );
    FREENULL( p_box->data.p_tfra->p_sample_number );
}