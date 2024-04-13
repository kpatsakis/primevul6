static int session_allow_incoming_new_stream(nghttp2_session *session) {
  return (session->goaway_flags &
          (NGHTTP2_GOAWAY_TERM_ON_SEND | NGHTTP2_GOAWAY_SENT)) == 0;
}