static int session_call_on_begin_frame(nghttp2_session *session,
                                       const nghttp2_frame_hd *hd) {
  int rv;

  if (session->callbacks.on_begin_frame_callback) {

    rv = session->callbacks.on_begin_frame_callback(session, hd,
                                                    session->user_data);

    if (rv != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }

  return 0;
}