int nghttp2_session_want_read(nghttp2_session *session) {
  size_t num_active_streams;

  /* If this flag is set, we don't want to read. The application
     should drop the connection. */
  if (session->goaway_flags & NGHTTP2_GOAWAY_TERM_SENT) {
    return 0;
  }

  num_active_streams = session_get_num_active_streams(session);

  /* Unless termination GOAWAY is sent or received, we always want to
     read incoming frames. */

  if (num_active_streams > 0) {
    return 1;
  }

  /* If there is no active streams and GOAWAY has been sent or
     received, we are done with this session. */
  return (session->goaway_flags &
          (NGHTTP2_GOAWAY_SENT | NGHTTP2_GOAWAY_RECV)) == 0;
}