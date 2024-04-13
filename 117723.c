int32_t nghttp2_session_get_stream_remote_window_size(nghttp2_session *session,
                                                      int32_t stream_id) {
  nghttp2_stream *stream;

  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream == NULL) {
    return -1;
  }

  /* stream->remote_window_size can be negative when
     SETTINGS_INITIAL_WINDOW_SIZE is changed. */
  return nghttp2_max(0, stream->remote_window_size);
}