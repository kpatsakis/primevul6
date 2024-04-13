static int session_process_extension_frame(nghttp2_session *session) {
  int rv;
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  rv = session_call_unpack_extension_callback(session);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  /* This handles the case where rv == NGHTTP2_ERR_CANCEL as well */
  if (rv != 0) {
    return 0;
  }

  return session_call_on_frame_received(session, frame);
}