static int session_terminate_session(nghttp2_session *session,
                                     int32_t last_stream_id,
                                     uint32_t error_code, const char *reason) {
  int rv;
  const uint8_t *debug_data;
  size_t debug_datalen;

  if (session->goaway_flags & NGHTTP2_GOAWAY_TERM_ON_SEND) {
    return 0;
  }

  /* Ignore all incoming frames because we are going to tear down the
     session. */
  session->iframe.state = NGHTTP2_IB_IGN_ALL;

  if (reason == NULL) {
    debug_data = NULL;
    debug_datalen = 0;
  } else {
    debug_data = (const uint8_t *)reason;
    debug_datalen = strlen(reason);
  }

  rv = nghttp2_session_add_goaway(session, last_stream_id, error_code,
                                  debug_data, debug_datalen,
                                  NGHTTP2_GOAWAY_AUX_TERM_ON_SEND);

  if (rv != 0) {
    return rv;
  }

  session->goaway_flags |= NGHTTP2_GOAWAY_TERM_ON_SEND;

  return 0;
}