static int session_process_origin_frame(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;
  nghttp2_mem *mem = &session->mem;
  int rv;

  rv = nghttp2_frame_unpack_origin_payload(&frame->ext, iframe->lbuf.pos,
                                           nghttp2_buf_len(&iframe->lbuf), mem);
  if (rv != 0) {
    if (nghttp2_is_fatal(rv)) {
      return rv;
    }
    /* Ignore ORIGIN frame which cannot be parsed. */
    return 0;
  }

  return nghttp2_session_on_origin_received(session, frame);
}