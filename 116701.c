relay_websocket_encode_frame (int opcode,
                              const char *buffer,
                              unsigned long long length,
                              unsigned long long *length_frame)
{
    unsigned char *frame;
    unsigned long long index;

    *length_frame = 0;

    frame = malloc (length + 10);
    if (!frame)
        return NULL;

    frame[0] = 0x80;
    frame[0] |= opcode;

    if (length <= 125)
    {
        /* length on one byte */
        frame[1] = length;
        index = 2;
    }
    else if (length <= 65535)
    {
        /* length on 2 bytes */
        frame[1] = 126;
        frame[2] = (length >> 8) & 0xFF;
        frame[3] = length & 0xFF;
        index = 4;
    }
    else
    {
        /* length on 8 bytes */
        frame[1] = 127;
        frame[2] = (length >> 56) & 0xFF;
        frame[3] = (length >> 48) & 0xFF;
        frame[4] = (length >> 40) & 0xFF;
        frame[5] = (length >> 32) & 0xFF;
        frame[6] = (length >> 24) & 0xFF;
        frame[7] = (length >> 16) & 0xFF;
        frame[8] = (length >> 8) & 0xFF;
        frame[9] = length & 0xFF;
        index = 10;
    }

    /* copy buffer after length */
    memcpy (frame + index, buffer, length);

    *length_frame = index + length;

    return (char *)frame;
}