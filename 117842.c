static int session_process_goaway_frame(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;

  nghttp2_frame_unpack_goaway_payload(&frame->goaway, iframe->sbuf.pos,
                                      iframe->lbuf.pos,
                                      nghttp2_buf_len(&iframe->lbuf));

  nghttp2_buf_wrap_init(&iframe->lbuf, NULL, 0);

  return nghttp2_session_on_goaway_received(session, frame);
}