session_call_on_extension_chunk_recv_callback(nghttp2_session *session,
                                              const uint8_t *data, size_t len) {
  int rv;
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  if (session->callbacks.on_extension_chunk_recv_callback) {
    rv = session->callbacks.on_extension_chunk_recv_callback(
        session, &frame->hd, data, len, session->user_data);
    if (rv == NGHTTP2_ERR_CANCEL) {
      return rv;
    }
    if (rv != 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }

  return 0;
}