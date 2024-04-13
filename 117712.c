int nghttp2_session_on_window_update_received(nghttp2_session *session,
                                              nghttp2_frame *frame) {
  if (frame->hd.stream_id == 0) {
    return session_on_connection_window_update_received(session, frame);
  } else {
    return session_on_stream_window_update_received(session, frame);
  }
}