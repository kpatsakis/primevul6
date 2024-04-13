static bool checkreturn buf_read(pb_istream_t *stream, pb_byte_t *buf, size_t count)
{
    size_t i;
    const pb_byte_t *source = (const pb_byte_t*)stream->state;
    stream->state = (pb_byte_t*)stream->state + count;
    
    if (buf != NULL)
    {
        for (i = 0; i < count; i++)
            buf[i] = source[i];
    }
    
    return true;
}