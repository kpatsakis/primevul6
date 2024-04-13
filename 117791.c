static int session_handle_frame_size_error(nghttp2_session *session) {
  /* TODO Currently no callback is called for this error, because we
     call this callback before reading any payload */
  return nghttp2_session_terminate_session(session, NGHTTP2_FRAME_SIZE_ERROR);
}