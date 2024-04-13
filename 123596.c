static int rtp_packetize_simple(sout_stream_id_sys_t *id, block_t *block)
{
    bool marker = (block->i_flags & BLOCK_FLAG_DISCONTINUITY) != 0;

    block = block_Realloc(block, 12, block->i_buffer);
    if (unlikely(block == NULL))
        return VLC_ENOMEM;

    rtp_packetize_common(id, block, marker, block->i_pts);
    rtp_packetize_send(id, block);
    return VLC_SUCCESS;
}