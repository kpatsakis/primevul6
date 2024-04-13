static int MP4_ReadBox_dec3( stream_t *p_stream, MP4_Box_t *p_box )
{
    MP4_READBOX_ENTER( MP4_Box_data_dec3_t );

    MP4_Box_data_dec3_t *p_dec3 = p_box->data.p_dec3;

    unsigned i_header;
    MP4_GET2BYTES( i_header );

    p_dec3->i_data_rate = i_header >> 3;
    p_dec3->i_num_ind_sub = (i_header & 0x7) + 1;
    for (uint8_t i = 0; i < p_dec3->i_num_ind_sub; i++) {
        MP4_GET3BYTES( i_header );
        p_dec3->stream[i].i_fscod = ( i_header >> 22 ) & 0x03;
        p_dec3->stream[i].i_bsid  = ( i_header >> 17 ) & 0x01f;
        p_dec3->stream[i].i_bsmod = ( i_header >> 12 ) & 0x01f;
        p_dec3->stream[i].i_acmod = ( i_header >> 9 ) & 0x07;
        p_dec3->stream[i].i_lfeon = ( i_header >> 8 ) & 0x01;
        p_dec3->stream[i].i_num_dep_sub = (i_header >> 1) & 0x0f;
        if (p_dec3->stream[i].i_num_dep_sub) {
            MP4_GET1BYTE( p_dec3->stream[i].i_chan_loc );
            p_dec3->stream[i].i_chan_loc |= (i_header & 1) << 8;
        } else
            p_dec3->stream[i].i_chan_loc = 0;
    }

#ifdef MP4_VERBOSE
    msg_Dbg( p_stream,
        "read box: \"dec3\" bitrate %dkbps %d independant substreams",
            p_dec3->i_data_rate, p_dec3->i_num_ind_sub);

    for (uint8_t i = 0; i < p_dec3->i_num_ind_sub; i++)
        msg_Dbg( p_stream,
                "\tstream %d: bsid=0x%x bsmod=0x%x acmod=0x%x lfeon=0x%x "
                "num dependant subs=%d chan_loc=0x%x",
                i, p_dec3->stream[i].i_bsid, p_dec3->stream[i].i_bsmod, p_dec3->stream[i].i_acmod,
                p_dec3->stream[i].i_lfeon, p_dec3->stream[i].i_num_dep_sub, p_dec3->stream[i].i_chan_loc );
#endif
    MP4_READBOX_EXIT( 1 );
}