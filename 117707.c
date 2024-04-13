static int session_handle_invalid_stream2(nghttp2_session *session,
                                          int32_t stream_id,
                                          nghttp2_frame *frame,
                                          int lib_error_code) {
  int rv;
  rv = nghttp2_session_add_rst_stream(
      session, stream_id, get_error_code_from_lib_error_code(lib_error_code));
  if (rv != 0) {
    return rv;
  }
  if (session->callbacks.on_invalid_frame_recv_callback) {
    if (session->callbacks.on_invalid_frame_recv_callback(
            session, frame, lib_error_code, session->user_data) != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
  return 0;
}