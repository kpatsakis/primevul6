bool pb_decode_svarint(pb_istream_t *stream, pb_int64_t *dest)
{
    pb_uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    if (value & 1)
        *dest = (pb_int64_t)(~(value >> 1));
    else
        *dest = (pb_int64_t)(value >> 1);
    
    return true;
}