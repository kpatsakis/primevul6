static int session_call_on_frame_send(nghttp2_session *session,
                                      nghttp2_frame *frame) {
  int rv;
  if (session->callbacks.on_frame_send_callback) {
    rv = session->callbacks.on_frame_send_callback(session, frame,
                                                   session->user_data);
    if (rv != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
  return 0;
}