static int session_inflate_handle_invalid_connection(nghttp2_session *session,
                                                     nghttp2_frame *frame,
                                                     int lib_error_code,
                                                     const char *reason) {
  int rv;
  rv =
      session_handle_invalid_connection(session, frame, lib_error_code, reason);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }
  return NGHTTP2_ERR_IGN_HEADER_BLOCK;
}