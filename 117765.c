static int session_call_on_frame_received(nghttp2_session *session,
                                          nghttp2_frame *frame) {
  int rv;
  if (session->callbacks.on_frame_recv_callback) {
    rv = session->callbacks.on_frame_recv_callback(session, frame,
                                                   session->user_data);
    if (rv != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
  return 0;
}