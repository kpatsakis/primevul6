static int session_process_data_frame(nghttp2_session *session) {
  int rv;
  nghttp2_frame *public_data_frame = &session->iframe.frame;
  rv = nghttp2_session_on_data_received(session, public_data_frame);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }
  return 0;
}