static int rtp_packetize_mpa( sout_stream_id_sys_t *id, block_t *in )
{
    int     i_max   = rtp_mtu (id) - 4; /* payload max in one packet */
    int     i_count = ( in->i_buffer + i_max - 1 ) / i_max;

    uint8_t *p_data = in->p_buffer;
    int     i_data  = in->i_buffer;
    int     i;

    for( i = 0; i < i_count; i++ )
    {
        int           i_payload = __MIN( i_max, i_data );
        block_t *out = block_Alloc( 16 + i_payload );

        /* rtp common header */
        rtp_packetize_common( id, out, (i == i_count - 1)?1:0, in->i_pts );
        /* mbz set to 0 */
        SetWBE( out->p_buffer + 12, 0 );
        /* fragment offset in the current frame */
        SetWBE( out->p_buffer + 14, i * i_max );
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