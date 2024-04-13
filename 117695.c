static int session_process_push_promise_frame(nghttp2_session *session) {
  int rv;
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  rv = nghttp2_frame_unpack_push_promise_payload(&frame->push_promise,
                                                 iframe->sbuf.pos);

  if (rv != 0) {
    return nghttp2_session_terminate_session_with_reason(
        session, NGHTTP2_PROTOCOL_ERROR, "PUSH_PROMISE: could not unpack");
  }

  return nghttp2_session_on_push_promise_received(session, frame);
}