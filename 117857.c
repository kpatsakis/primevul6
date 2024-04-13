session_predicate_push_response_headers_send(nghttp2_session *session,
                                             nghttp2_stream *stream) {
  int rv;
  /* TODO Should disallow HEADERS if GOAWAY has already been issued? */
  rv = session_predicate_for_stream_send(session, stream);
  if (rv != 0) {
    return rv;
  }
  assert(stream);
  if (!session->server) {
    return NGHTTP2_ERR_PROTO;
  }
  if (stream->state != NGHTTP2_STREAM_RESERVED) {
    return NGHTTP2_ERR_PROTO;
  }
  if (session->goaway_flags & NGHTTP2_GOAWAY_RECV) {
    return NGHTTP2_ERR_START_STREAM_NOT_ALLOWED;
  }
  return 0;
}