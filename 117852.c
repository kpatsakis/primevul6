static int session_predicate_for_stream_send(nghttp2_session *session,
                                             nghttp2_stream *stream) {
  if (stream == NULL) {
    return NGHTTP2_ERR_STREAM_CLOSED;
  }
  if (session_is_closing(session)) {
    return NGHTTP2_ERR_SESSION_CLOSING;
  }
  if (stream->shut_flags & NGHTTP2_SHUT_WR) {
    return NGHTTP2_ERR_STREAM_SHUT_WR;
  }
  return 0;
}