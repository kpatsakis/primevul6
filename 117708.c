static int nghttp2_session_predicate_data_send(nghttp2_session *session,
                                               nghttp2_stream *stream) {
  int rv;
  rv = session_predicate_for_stream_send(session, stream);
  if (rv != 0) {
    return rv;
  }
  assert(stream);
  if (nghttp2_session_is_my_stream_id(session, stream->stream_id)) {
    /* Request body data */
    /* If stream->state is NGHTTP2_STREAM_CLOSING, RST_STREAM was
       queued but not yet sent. In this case, we won't send DATA
       frames. */
    if (stream->state == NGHTTP2_STREAM_CLOSING) {
      return NGHTTP2_ERR_STREAM_CLOSING;
    }
    if (stream->state == NGHTTP2_STREAM_RESERVED) {
      return NGHTTP2_ERR_INVALID_STREAM_STATE;
    }
    return 0;
  }
  /* Response body data */
  if (stream->state == NGHTTP2_STREAM_OPENED) {
    return 0;
  }
  if (stream->state == NGHTTP2_STREAM_CLOSING) {
    return NGHTTP2_ERR_STREAM_CLOSING;
  }
  return NGHTTP2_ERR_INVALID_STREAM_STATE;
}