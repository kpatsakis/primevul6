int nghttp2_session_check_request_allowed(nghttp2_session *session) {
  return !session->server && session->next_stream_id <= INT32_MAX &&
         (session->goaway_flags & NGHTTP2_GOAWAY_RECV) == 0 &&
         !session_is_closing(session);
}