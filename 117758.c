static size_t nghttp2_session_next_data_read(nghttp2_session *session,
                                             nghttp2_stream *stream) {
  ssize_t window_size;

  window_size = nghttp2_session_enforce_flow_control_limits(
      session, stream, NGHTTP2_DATA_PAYLOADLEN);

  DEBUGF("send: available window=%zd\n", window_size);

  return window_size > 0 ? (size_t)window_size : 0;
}