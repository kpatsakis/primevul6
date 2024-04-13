bool checkreturn pb_skip_varint(pb_istream_t *stream)
{
    pb_byte_t byte;
    do
    {
        if (!pb_read(stream, &byte, 1))
            return false;
    } while (byte & 0x80);
    return true;
}