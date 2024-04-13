static int session_call_on_begin_headers(nghttp2_session *session,
                                         nghttp2_frame *frame) {
  int rv;
  DEBUGF("recv: call on_begin_headers callback stream_id=%d\n",
         frame->hd.stream_id);
  if (session->callbacks.on_begin_headers_callback) {
    rv = session->callbacks.on_begin_headers_callback(session, frame,
                                                      session->user_data);
    if (rv == NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE) {
      return rv;
    }
    if (rv != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
  return 0;
}