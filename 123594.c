int rtp_packetize_xiph_config( sout_stream_id_sys_t *id, const char *fmtp,
                               int64_t i_pts )
{
    if (fmtp == NULL)
        return VLC_EGENERIC;

    /* extract base64 configuration from fmtp */
    char *start = strstr(fmtp, "configuration=");
    assert(start != NULL);
    start += sizeof("configuration=") - 1;
    char *end = strchr(start, ';');
    assert(end != NULL);
    size_t len = end - start;

    char *b64 = malloc(len + 1);
    if(!b64)
        return VLC_EGENERIC;

    memcpy(b64, start, len);
    b64[len] = '\0';

    int     i_max   = rtp_mtu (id) - 6; /* payload max in one packet */

    uint8_t *p_orig, *p_data;
    int i_data;

    i_data = vlc_b64_decode_binary(&p_orig, b64);
    free(b64);
    if (i_data <= 9)
    {
        free(p_orig);
        return VLC_EGENERIC;
    }
    p_data = p_orig + 9;
    i_data -= 9;

    int i_count = ( i_data + i_max - 1 ) / i_max;

    for( int i = 0; i < i_count; i++ )
    {
        int           i_payload = __MIN( i_max, i_data );
        block_t *out = block_Alloc( 18 + i_payload );

        unsigned fragtype, numpkts;
        if (i_count == 1)
        {
            fragtype = 0;
            numpkts = 1;
        }
        else
        {
            numpkts = 0;
            if (i == 0)
                fragtype = 1;
            else if (i == i_count - 1)
                fragtype = 3;
            else
                fragtype = 2;
        }
        /* Ident:24, Fragment type:2, Vorbis/Theora Data Type:2, # of pkts:4 */
        uint32_t header = ((XIPH_IDENT & 0xffffff) << 8) |
                          (fragtype << 6) | (1 << 4) | numpkts;

        /* rtp common header */
        rtp_packetize_common( id, out, 0, i_pts );

        SetDWBE( out->p_buffer + 12, header);
        SetWBE( out->p_buffer + 16, i_payload);
        memcpy( &out->p_buffer[18], p_data, i_payload );

        out->i_dts    = i_pts;

        rtp_packetize_send( id, out );

        p_data += i_payload;
        i_data -= i_payload;
    }

    free(p_orig);

    return VLC_SUCCESS;
}