static int session_process_ping_frame(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  nghttp2_frame_unpack_ping_payload(&frame->ping, iframe->sbuf.pos);

  return nghttp2_session_on_ping_received(session, frame);
}