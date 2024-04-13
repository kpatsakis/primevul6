static int session_predicate_headers_send(nghttp2_session *session,
                                          nghttp2_stream *stream) {
  int rv;
  rv = session_predicate_for_stream_send(session, stream);
  if (rv != 0) {
    return rv;
  }
  assert(stream);

  switch (stream->state) {
  case NGHTTP2_STREAM_OPENED:
    return 0;
  case NGHTTP2_STREAM_CLOSING:
    return NGHTTP2_ERR_STREAM_CLOSING;
  default:
    if (nghttp2_session_is_my_stream_id(session, stream->stream_id)) {
      return 0;
    }
    return NGHTTP2_ERR_INVALID_STREAM_STATE;
  }
}