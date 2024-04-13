bool checkreturn pb_decode_varint32(pb_istream_t *stream, uint32_t *dest)
{
    return pb_decode_varint32_eof(stream, dest, NULL);
}