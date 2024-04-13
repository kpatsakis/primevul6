bool checkreturn pb_close_string_substream(pb_istream_t *stream, pb_istream_t *substream)
{
    if (substream->bytes_left) {
        if (!pb_read(substream, NULL, substream->bytes_left))
            return false;
    }

    stream->state = substream->state;

#ifndef PB_NO_ERRMSG
    stream->errmsg = substream->errmsg;
#endif
    return true;
}