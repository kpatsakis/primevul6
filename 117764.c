static int session_call_before_frame_send(nghttp2_session *session,
                                          nghttp2_frame *frame) {
  int rv;
  if (session->callbacks.before_frame_send_callback) {
    rv = session->callbacks.before_frame_send_callback(session, frame,
                                                       session->user_data);
    if (rv == NGHTTP2_ERR_CANCEL) {
      return rv;
    }

    if (rv != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
  return 0;
}