static int session_process_priority_frame(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  nghttp2_frame_unpack_priority_payload(&frame->priority, iframe->sbuf.pos);

  return nghttp2_session_on_priority_received(session, frame);
}