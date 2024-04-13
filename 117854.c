int nghttp2_session_get_stream_local_close(nghttp2_session *session,
                                           int32_t stream_id) {
  nghttp2_stream *stream;

  stream = nghttp2_session_get_stream(session, stream_id);

  if (!stream) {
    return -1;
  }

  return (stream->shut_flags & NGHTTP2_SHUT_WR) != 0;
}