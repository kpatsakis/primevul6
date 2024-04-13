static int session_handle_invalid_connection(nghttp2_session *session,
                                             nghttp2_frame *frame,
                                             int lib_error_code,
                                             const char *reason) {
  if (session->callbacks.on_invalid_frame_recv_callback) {
    if (session->callbacks.on_invalid_frame_recv_callback(
            session, frame, lib_error_code, session->user_data) != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
  return nghttp2_session_terminate_session_with_reason(
      session, get_error_code_from_lib_error_code(lib_error_code), reason);
}