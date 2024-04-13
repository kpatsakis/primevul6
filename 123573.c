static int rtp_packetize_g726_32( sout_stream_id_sys_t *id, block_t *in )
{
    return rtp_packetize_g726( id, in, 2 );
}