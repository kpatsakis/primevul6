static int session_inflate_handle_invalid_stream(nghttp2_session *session,
                                                 nghttp2_frame *frame,
                                                 int lib_error_code) {
  int rv;
  rv = session_handle_invalid_stream(session, frame, lib_error_code);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }
  return NGHTTP2_ERR_IGN_HEADER_BLOCK;
}