int32_t nghttp2_session_get_stream_local_window_size(nghttp2_session *session,
                                                     int32_t stream_id) {
  nghttp2_stream *stream;
  int32_t size;
  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream == NULL) {
    return -1;
  }

  size = stream->local_window_size - stream->recv_window_size;

  /* size could be negative if local endpoint reduced
     SETTINGS_INITIAL_WINDOW_SIZE */
  if (size < 0) {
    return 0;
  }

  return size;
}