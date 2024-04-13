static int session_predicate_altsvc_send(nghttp2_session *session,
                                         int32_t stream_id) {
  nghttp2_stream *stream;

  if (session_is_closing(session)) {
    return NGHTTP2_ERR_SESSION_CLOSING;
  }

  if (stream_id == 0) {
    return 0;
  }

  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream == NULL) {
    return NGHTTP2_ERR_STREAM_CLOSED;
  }
  if (stream->state == NGHTTP2_STREAM_CLOSING) {
    return NGHTTP2_ERR_STREAM_CLOSING;
  }

  return 0;
}