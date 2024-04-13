static int session_predicate_push_promise_send(nghttp2_session *session,
                                               nghttp2_stream *stream) {
  int rv;

  if (!session->server) {
    return NGHTTP2_ERR_PROTO;
  }

  rv = session_predicate_for_stream_send(session, stream);
  if (rv != 0) {
    return rv;
  }

  assert(stream);

  if (session->remote_settings.enable_push == 0) {
    return NGHTTP2_ERR_PUSH_DISABLED;
  }
  if (stream->state == NGHTTP2_STREAM_CLOSING) {
    return NGHTTP2_ERR_STREAM_CLOSING;
  }
  if (session->goaway_flags & NGHTTP2_GOAWAY_RECV) {
    return NGHTTP2_ERR_START_STREAM_NOT_ALLOWED;
  }
  return 0;
}