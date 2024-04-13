void *nghttp2_session_get_stream_user_data(nghttp2_session *session,
                                           int32_t stream_id) {
  nghttp2_stream *stream;
  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream) {
    return stream->stream_user_data;
  } else {
    return NULL;
  }
}