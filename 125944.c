static void MP4_FreeBox_tref_generic( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_tref_generic->i_track_ID );
}