static int session_predicate_origin_send(nghttp2_session *session) {
  if (session_is_closing(session)) {
    return NGHTTP2_ERR_SESSION_CLOSING;
  }
  return 0;
}