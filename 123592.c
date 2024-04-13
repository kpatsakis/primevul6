static int rtp_packetize_t140( sout_stream_id_sys_t *id, block_t *in )
{
    const size_t   i_max  = rtp_mtu (id);
    const uint8_t *p_data = in->p_buffer;
    size_t         i_data = in->i_buffer;

    for( unsigned i_packet = 0; i_data > 0; i_packet++ )
    {
        size_t i_payload = i_data;

        /* Make sure we stop on an UTF-8 character boundary
         * (assuming the input is valid UTF-8) */
        if( i_data > i_max )
        {
            i_payload = i_max;

            while( ( p_data[i_payload] & 0xC0 ) == 0x80 )
            {
                if( i_payload == 0 )
                 {
                    block_Release(in);
                    return VLC_SUCCESS; /* fishy input! */
                }
                i_payload--;
            }
        }

        block_t *out = block_Alloc( 12 + i_payload );
        if( out == NULL )
        {
            block_Release(in);
            return VLC_SUCCESS;
        }

        rtp_packetize_common( id, out, 0, in->i_pts + i_packet );
        memcpy( out->p_buffer + 12, p_data, i_payload );

        out->i_dts    = in->i_pts;
        out->i_length = 0;

        rtp_packetize_send( id, out );

        p_data += i_payload;
        i_data -= i_payload;
    }

    block_Release(in);
    return VLC_SUCCESS;
}