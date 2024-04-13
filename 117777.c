static int session_call_unpack_extension_callback(nghttp2_session *session) {
  int rv;
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;
  void *payload = NULL;

  rv = session->callbacks.unpack_extension_callback(
      session, &payload, &frame->hd, session->user_data);
  if (rv == NGHTTP2_ERR_CANCEL) {
    return rv;
  }
  if (rv != 0) {
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  frame->ext.payload = payload;

  return 0;
}