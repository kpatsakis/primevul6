static int rtp_packetize_g726( sout_stream_id_sys_t *id, block_t *in, int i_pad )
{
    int     i_max   = (rtp_mtu( id )- 12 + i_pad - 1) & ~i_pad;
    int     i_count = ( in->i_buffer + i_max - 1 ) / i_max;

    uint8_t *p_data = in->p_buffer;
    int     i_data  = in->i_buffer;
    int     i_packet = 0;

    while( i_data > 0 )
    {
        int           i_payload = __MIN( i_max, i_data );
        block_t *out = block_Alloc( 12 + i_payload );

        /* rtp common header */
        rtp_packetize_common( id, out, 0,
                      (in->i_pts > VLC_TS_INVALID ? in->i_pts : in->i_dts) );

        memcpy( &out->p_buffer[12], p_data, i_payload );

        out->i_dts    = in->i_dts + i_packet++ * in->i_length / i_count;
        out->i_length = in->i_length / i_count;

        rtp_packetize_send( id, out );

        p_data += i_payload;
        i_data -= i_payload;
    }

    block_Release(in);
    return VLC_SUCCESS;
}