quoted_string_istream_read_parent(struct quoted_string_istream *qsstream,
				  unsigned int min_bytes)
{
	struct istream_private *stream = &qsstream->istream;
	size_t size, avail;
	ssize_t ret;

	size = i_stream_get_data_size(stream->parent);
	while (size < min_bytes) {
		ret = i_stream_read_memarea(stream->parent);
		if (ret <= 0) {
			if (ret == -2) {
				/* tiny parent buffer size - shouldn't happen */
				return -2;
			}
			stream->istream.stream_errno =
				stream->parent->stream_errno;
			stream->istream.eof = stream->parent->eof;
			if (ret == -1 && stream->istream.stream_errno == 0) {
				io_stream_set_error(&stream->iostream,
					"Quoted string ends without closing quotes");
				stream->istream.stream_errno = EPIPE;
			}
			return ret;
		}
		size = i_stream_get_data_size(stream->parent);
	}

	if (!i_stream_try_alloc(stream, size, &avail))
		return -2;
	return 1;
}