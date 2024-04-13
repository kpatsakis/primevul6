static int rtp_packetize_mpv( sout_stream_id_sys_t *id, block_t *in )
{
    int     i_max   = rtp_mtu (id) - 4; /* payload max in one packet */
    int     i_count = ( in->i_buffer + i_max - 1 ) / i_max;

    uint8_t *p_data = in->p_buffer;
    int     i_data  = in->i_buffer;
    int     i;
    int     b_sequence_start = 0;
    int     i_temporal_ref = 0;
    int     i_picture_coding_type = 0;
    int     i_fbv = 0, i_bfc = 0, i_ffv = 0, i_ffc = 0;
    int     b_start_slice = 0;

    /* preparse this packet to get some info */
    if( in->i_buffer > 4 )
    {
        uint8_t *p = p_data;
        int      i_rest = in->i_buffer;

        for( ;; )
        {
            while( i_rest > 4 &&
                   ( p[0] != 0x00 || p[1] != 0x00 || p[2] != 0x01 ) )
            {
                p++;
                i_rest--;
            }
            if( i_rest <= 4 )
            {
                break;
            }
            p += 3;
            i_rest -= 4;

            if( *p == 0xb3 )
            {
                /* sequence start code */
                b_sequence_start = 1;
            }
            else if( *p == 0x00 && i_rest >= 4 )
            {
                /* picture */
                i_temporal_ref = ( p[1] << 2) |((p[2]>>6)&0x03);
                i_picture_coding_type = (p[2] >> 3)&0x07;

                if( i_rest >= 4 && ( i_picture_coding_type == 2 ||
                                    i_picture_coding_type == 3 ) )
                {
                    i_ffv = (p[3] >> 2)&0x01;
                    i_ffc = ((p[3]&0x03) << 1)|((p[4]>>7)&0x01);
                    if( i_rest > 4 && i_picture_coding_type == 3 )
                    {
                        i_fbv = (p[4]>>6)&0x01;
                        i_bfc = (p[4]>>3)&0x07;
                    }
                }
            }
            else if( *p <= 0xaf )
            {
                b_start_slice = 1;
            }
        }
    }

    for( i = 0; i < i_count; i++ )
    {
        int           i_payload = __MIN( i_max, i_data );
        block_t *out = block_Alloc( 16 + i_payload );
        /* MBZ:5 T:1 TR:10 AN:1 N:1 S:1 B:1 E:1 P:3 FBV:1 BFC:3 FFV:1 FFC:3 */
        uint32_t      h = ( i_temporal_ref << 16 )|
                          ( b_sequence_start << 13 )|
                          ( b_start_slice << 12 )|
                          ( i == i_count - 1 ? 1 << 11 : 0 )|
                          ( i_picture_coding_type << 8 )|
                          ( i_fbv << 7 )|( i_bfc << 4 )|( i_ffv << 3 )|i_ffc;

        /* rtp common header */
        rtp_packetize_common( id, out, (i == i_count - 1)?1:0,
                          in->i_pts > VLC_TS_INVALID ? in->i_pts : in->i_dts );

        SetDWBE( out->p_buffer + 12, h );

        memcpy( &out->p_buffer[16], p_data, i_payload );

        out->i_dts    = in->i_dts + i * in->i_length / i_count;
        out->i_length = in->i_length / i_count;

        rtp_packetize_send( id, out );

        p_data += i_payload;
        i_data -= i_payload;
    }

    block_Release(in);
    return VLC_SUCCESS;
}