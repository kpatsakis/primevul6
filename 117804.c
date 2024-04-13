static int session_call_on_invalid_frame_recv_callback(nghttp2_session *session,
                                                       nghttp2_frame *frame,
                                                       int lib_error_code) {
  if (session->callbacks.on_invalid_frame_recv_callback) {
    if (session->callbacks.on_invalid_frame_recv_callback(
            session, frame, lib_error_code, session->user_data) != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
  return 0;
}