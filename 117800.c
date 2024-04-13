int nghttp2_session_on_ping_received(nghttp2_session *session,
                                     nghttp2_frame *frame) {
  int rv = 0;
  if (frame->hd.stream_id != 0) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "PING: stream_id != 0");
  }
  if ((session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_PING_ACK) == 0 &&
      (frame->hd.flags & NGHTTP2_FLAG_ACK) == 0 &&
      !session_is_closing(session)) {
    /* Peer sent ping, so ping it back */
    rv = nghttp2_session_add_ping(session, NGHTTP2_FLAG_ACK,
                                  frame->ping.opaque_data);
    if (rv != 0) {
      return rv;
    }
  }
  return session_call_on_frame_received(session, frame);
}