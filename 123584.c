rtp_packetize_h264_nal( sout_stream_id_sys_t *id,
                        const uint8_t *p_data, int i_data, int64_t i_pts,
                        int64_t i_dts, bool b_last, int64_t i_length )
{
    const int i_max = rtp_mtu (id); /* payload max in one packet */
    int i_nal_hdr;
    int i_nal_type;

    if( i_data < 5 )
        return VLC_SUCCESS;

    i_nal_hdr = p_data[3];
    i_nal_type = i_nal_hdr&0x1f;

    /* Skip start code */
    p_data += 3;
    i_data -= 3;

    /* */
    if( i_data <= i_max )
    {
        /* Single NAL unit packet */
        block_t *out = block_Alloc( 12 + i_data );
        out->i_dts    = i_dts;
        out->i_length = i_length;

        /* */
        rtp_packetize_common( id, out, b_last, i_pts );

        memcpy( &out->p_buffer[12], p_data, i_data );

        rtp_packetize_send( id, out );
    }
    else
    {
        /* FU-A Fragmentation Unit without interleaving */
        const int i_count = ( i_data-1 + i_max-2 - 1 ) / (i_max-2);
        int i;

        p_data++;
        i_data--;

        for( i = 0; i < i_count; i++ )
        {
            const int i_payload = __MIN( i_data, i_max-2 );
            block_t *out = block_Alloc( 12 + 2 + i_payload );
            out->i_dts    = i_dts + i * i_length / i_count;
            out->i_length = i_length / i_count;

            /* */
            rtp_packetize_common( id, out, (b_last && i_payload == i_data),
                                    i_pts );
            /* FU indicator */
            out->p_buffer[12] = 0x00 | (i_nal_hdr & 0x60) | 28;
            /* FU header */
            out->p_buffer[13] = ( i == 0 ? 0x80 : 0x00 ) | ( (i == i_count-1) ? 0x40 : 0x00 )  | i_nal_type;
            memcpy( &out->p_buffer[14], p_data, i_payload );

            rtp_packetize_send( id, out );

            i_data -= i_payload;
            p_data += i_payload;
        }
    }
    return VLC_SUCCESS;
}