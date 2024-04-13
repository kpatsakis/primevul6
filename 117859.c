static int session_is_closing(nghttp2_session *session) {
  return (session->goaway_flags & NGHTTP2_GOAWAY_TERM_ON_SEND) != 0 ||
         (nghttp2_session_want_read(session) == 0 &&
          nghttp2_session_want_write(session) == 0);
}