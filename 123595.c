static int rtp_packetize_jpeg( sout_stream_id_sys_t *id, block_t *in )
{
    uint8_t *p_data = in->p_buffer;
    int      i_data = in->i_buffer;
    uint8_t *bufend = p_data + i_data;

    const uint8_t *qtables = NULL;
    int nb_qtables = 0;
    int off = 0; // fragment offset in frame
    int y_sampling_factor;
    // type is set by pixel format (determined by y_sampling_factor):
    // 0 for yuvj422p
    // 1 for yuvj420p
    // += 64 if DRI present
    int type;
    int w = 0; // Width in multiples of 8
    int h = 0; // Height in multiples of 8
    int restart_interval;
    int dri_found = 0;

    // Skip SOI
    if (GetWBE(p_data) != 0xffd8)
        goto error;
    p_data += 2;
    i_data -= 2;

    /* parse the header to get necessary info */
    int header_finished = 0;
    while (!header_finished && p_data + 4 <= bufend) {
        uint16_t marker = GetWBE(p_data);
        uint8_t *section = p_data + 2;
        int section_size = GetWBE(section);
        uint8_t *section_body = p_data + 4;
        if (section + section_size > bufend)
            goto error;

        assert((marker & 0xff00) == 0xff00);
        switch (marker)
        {
            case 0xffdb /*DQT*/:
                if (section_body[0])
                    goto error; // Only 8-bit precision is supported

                /* a quantization table is 64 bytes long */
                nb_qtables = section_size / 65;
                qtables = section_body;
                break;
            case 0xffc0 /*SOF0*/:
            {
                int height = GetWBE(&section_body[1]);
                int width = GetWBE(&section_body[3]);
                if (width > 2040 || height > 2040)
                {
                    // larger than limit supported by RFC 2435
                    goto error;
                }
                // Round up by 8, divide by 8
                w = ((width+7)&~7) >> 3;
                h = ((height+7)&~7) >> 3;

                // Get components sampling to determine type
                // Y has component ID 1
                // Possible configurations of sampling factors:
                // Y - 0x22, Cb - 0x11, Cr - 0x11 => yuvj420p
                // Y - 0x21, Cb - 0x11, Cr - 0x11 => yuvj422p

                // Only 3 components are supported by RFC 2435
                if (section_body[5] != 3) // Number of components
                    goto error;
                for (int j = 0; j < 3; j++)
                {
                    if (section_body[6 + j * 3] == 1 /* Y */)
                    {
                        y_sampling_factor = section_body[6 + j * 3 + 1];
                    }
                    else if (section_body[6 + j * 3 + 1] != 0x11)
                    {
                        // Sampling factor is unsupported by RFC 2435
                        goto error;
                    }
                }
                break;
            }
            case 0xffdd /*DRI*/:
                restart_interval = GetWBE(section_body);
                dri_found = 1;
                break;
            case 0xffda /*SOS*/:
                /* SOS is last marker in the header */
                header_finished = 1;
                break;
        }
        // Step over marker, 16bit length and section body
        p_data += 2 + section_size;
        i_data -= 2 + section_size;
    }
    if (!header_finished)
        goto error;
    if (!w || !h)
        goto error;

    switch (y_sampling_factor)
    {
        case 0x22: // yuvj420p
            type = 1;
            break;
        case 0x21: // yuvj422p
            type = 0;
            break;
        default:
            goto error; // Sampling format unsupported by RFC 2435
    }

    if (dri_found)
        type += 64;

    while ( i_data )
    {
        int hdr_size = 8 + dri_found * 4;
        if (off == 0 && nb_qtables)
            hdr_size += 4 + 64 * nb_qtables;

        int i_payload = __MIN( i_data, (int)(rtp_mtu (id) - hdr_size) );
        if ( i_payload <= 0 )
            return VLC_EGENERIC;

        block_t *out = block_Alloc( 12 + hdr_size + i_payload );
        if( out == NULL )
            return VLC_ENOMEM;

        uint8_t *p = out->p_buffer + 12;
        /* set main header */
        /* set type-specific=0, set offset in following 24 bits: */
        SetDWBE(p, off & 0x00ffffff);
        p += 4;
        *p++ = type;
        *p++ = 255;  // Quant value
        *p++ = w;
        *p++ = h;

        // Write restart_marker_hdr if needed
        if (dri_found)
        {
            SetWBE(p, restart_interval);
            p += 2;
            // restart_count. Hardcoded same value as in gstreamer implementation
            SetWBE(p, 0xffff);
            p += 2;
        }

        if (off == 0 && nb_qtables)
        {
            /* set quantization tables header */
            *p++ = 0;
            *p++ = 0;
            SetWBE (p, 64 * nb_qtables);
            p += 2;
            for (int i = 0; i < nb_qtables; i++)
            {
                memcpy (p, &qtables[65 * i + 1], 64);
                p += 64;
            }
        }

        /* rtp common header */
        rtp_packetize_common( id, out, (i_payload == i_data),
                      (in->i_pts > VLC_TS_INVALID ? in->i_pts : in->i_dts) );
        memcpy( p, p_data, i_payload );

        out->i_dts    = in->i_dts;
        out->i_length = in->i_length;

        rtp_packetize_send( id, out );

        p_data += i_payload;
        i_data -= i_payload;
        off    += i_payload;
    }

    block_Release(in);
    return VLC_SUCCESS;
error:
    block_Release(in);
    return VLC_EGENERIC;
}