int nghttp2_session_on_origin_received(nghttp2_session *session,
                                       nghttp2_frame *frame) {
  return session_call_on_frame_received(session, frame);
}