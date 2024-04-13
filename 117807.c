nghttp2_session_get_stream_effective_recv_data_length(nghttp2_session *session,
                                                      int32_t stream_id) {
  nghttp2_stream *stream;
  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream == NULL) {
    return -1;
  }
  return stream->recv_window_size < 0 ? 0 : stream->recv_window_size;
}