static void MP4_FreeBox_stco_co64( MP4_Box_t *p_box )
{
    FREENULL( p_box->data.p_co64->i_chunk_offset );
}