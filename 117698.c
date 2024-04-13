static int session_process_rst_stream_frame(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  nghttp2_frame_unpack_rst_stream_payload(&frame->rst_stream, iframe->sbuf.pos);

  return nghttp2_session_on_rst_stream_received(session, frame);
}