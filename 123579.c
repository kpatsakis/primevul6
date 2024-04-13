static int rtp_packetize_h264( sout_stream_id_sys_t *id, block_t *in )
{
    const uint8_t *p_buffer = in->p_buffer;
    int i_buffer = in->i_buffer;

    while( i_buffer > 4 && ( p_buffer[0] != 0 || p_buffer[1] != 0 || p_buffer[2] != 1 ) )
    {
        i_buffer--;
        p_buffer++;
    }

    /* Split nal units */
    while( i_buffer > 4 )
    {
        int i_offset;
        int i_size = i_buffer;
        int i_skip = i_buffer;

        /* search nal end */
        for( i_offset = 4; i_offset+2 < i_buffer ; i_offset++)
        {
            if( p_buffer[i_offset] == 0 && p_buffer[i_offset+1] == 0 && p_buffer[i_offset+2] == 1 )
            {
                /* we found another startcode */
                i_size = i_offset - ( p_buffer[i_offset-1] == 0 ? 1 : 0);
                i_skip = i_offset;
                break;
            }
        }
        /* TODO add STAP-A to remove a lot of overhead with small slice/sei/... */
        rtp_packetize_h264_nal( id, p_buffer, i_size,
                (in->i_pts > VLC_TS_INVALID ? in->i_pts : in->i_dts), in->i_dts,
                (i_size >= i_buffer), in->i_length * i_size / in->i_buffer );

        i_buffer -= i_skip;
        p_buffer += i_skip;
    }

    block_Release(in);
    return VLC_SUCCESS;
}