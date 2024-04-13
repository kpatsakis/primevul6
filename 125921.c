static int MP4_ReadBox_dac3( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_Box_data_dac3_t *p_dac3;
    MP4_READBOX_ENTER( MP4_Box_data_dac3_t );

    p_dac3 = p_box->data.p_dac3;

    unsigned i_header;
    MP4_GET3BYTES( i_header );

    p_dac3->i_fscod = ( i_header >> 22 ) & 0x03;
    p_dac3->i_bsid  = ( i_header >> 17 ) & 0x01f;
    p_dac3->i_bsmod = ( i_header >> 14 ) & 0x07;
    p_dac3->i_acmod = ( i_header >> 11 ) & 0x07;
    p_dac3->i_lfeon = ( i_header >> 10 ) & 0x01;
    p_dac3->i_bitrate_code = ( i_header >> 5) & 0x1f;

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
             "read box: \"dac3\" fscod=0x%x bsid=0x%x bsmod=0x%x acmod=0x%x lfeon=0x%x bitrate_code=0x%x",
             p_dac3->i_fscod, p_dac3->i_bsid, p_dac3->i_bsmod, p_dac3->i_acmod, p_dac3->i_lfeon, p_dac3->i_bitrate_code );
#endif
    MP4_READBOX_EXIT( 1 );
}