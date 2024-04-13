static int rtp_packetize_pcm(sout_stream_id_sys_t *id, block_t *in)
{
    unsigned max = rtp_mtu(id);

    while (in->i_buffer > max)
    {
        unsigned duration = (in->i_length * max) / in->i_buffer;
        bool marker = (in->i_flags & BLOCK_FLAG_DISCONTINUITY) != 0;

        block_t *out = block_Alloc(12 + max);
        if (unlikely(out == NULL))
        {
            block_Release(in);
            return VLC_ENOMEM;
        }

        rtp_packetize_common(id, out, marker, in->i_pts);
        memcpy(out->p_buffer + 12, in->p_buffer, max);
        rtp_packetize_send(id, out);

        in->p_buffer += max;
        in->i_buffer -= max;
        in->i_pts += duration;
        in->i_length -= duration;
        in->i_flags &= ~BLOCK_FLAG_DISCONTINUITY;
    }

    return rtp_packetize_simple(id, in); /* zero copy for the last frame */
}