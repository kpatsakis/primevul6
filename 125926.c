static void MP4_FreeBox_ftyp( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_ftyp->i_compatible_brands );
}