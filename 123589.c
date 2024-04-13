static int rtp_packetize_swab(sout_stream_id_sys_t *id, block_t *in)
{
    unsigned max = rtp_mtu(id);

    while (in->i_buffer > 0)
    {
        unsigned payload = (max < in->i_buffer) ? max : in->i_buffer;
        unsigned duration = (in->i_length * payload) / in->i_buffer;
        bool marker = (in->i_flags & BLOCK_FLAG_DISCONTINUITY) != 0;

        block_t *out = block_Alloc(12 + payload);
        if (unlikely(out == NULL))
        {
            block_Release(in);
            return VLC_ENOMEM;
        }

        rtp_packetize_common(id, out, marker, in->i_pts);
        swab(in->p_buffer, out->p_buffer + 12, payload);
        rtp_packetize_send(id, out);

        in->p_buffer += payload;
        in->i_buffer -= payload;
        in->i_pts += duration;
        in->i_length -= duration;
        in->i_flags &= ~BLOCK_FLAG_DISCONTINUITY;
    }

    block_Release(in);
    return VLC_SUCCESS;
}