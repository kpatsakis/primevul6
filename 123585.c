static int rtp_packetize_g726_24( sout_stream_id_sys_t *id, block_t *in )
{
    return rtp_packetize_g726( id, in, 8 );
}