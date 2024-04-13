static int session_process_window_update_frame(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  nghttp2_frame_unpack_window_update_payload(&frame->window_update,
                                             iframe->sbuf.pos);

  return nghttp2_session_on_window_update_received(session, frame);
}