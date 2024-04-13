static int MP4_ReadBox_vmhd( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_vmhd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_vmhd );

    MP4_GET2BYTES( p_box->data.p_vmhd->i_graphics_mode );
    for( unsigned i = 0; i < 3; i++ )
    {
        MP4_GET2BYTES( p_box->data.p_vmhd->i_opcolor[i] );
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "read box: \"vmhd\" graphics-mode %d opcolor (%d, %d, %d)",
                      p_box->data.p_vmhd->i_graphics_mode,
                      p_box->data.p_vmhd->i_opcolor[0],
                      p_box->data.p_vmhd->i_opcolor[1],
                      p_box->data.p_vmhd->i_opcolor[2] );
#endif
    MP4_READBOX_EXIT( 1 );
}