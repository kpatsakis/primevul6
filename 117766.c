nghttp2_session_get_stream_effective_local_window_size(nghttp2_session *session,
                                                       int32_t stream_id) {
  nghttp2_stream *stream;
  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream == NULL) {
    return -1;
  }
  return stream->local_window_size;
}