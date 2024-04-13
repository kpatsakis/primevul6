static int session_predicate_request_headers_send(nghttp2_session *session,
                                                  nghttp2_outbound_item *item) {
  if (item->aux_data.headers.canceled) {
    return NGHTTP2_ERR_STREAM_CLOSING;
  }
  /* If we are terminating session (NGHTTP2_GOAWAY_TERM_ON_SEND),
     GOAWAY was received from peer, or session is about to close, new
     request is not allowed. */
  if ((session->goaway_flags & NGHTTP2_GOAWAY_RECV) ||
      session_is_closing(session)) {
    return NGHTTP2_ERR_START_STREAM_NOT_ALLOWED;
  }
  return 0;
}