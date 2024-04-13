session_on_connection_window_update_received(nghttp2_session *session,
                                             nghttp2_frame *frame) {
  /* Handle connection-level flow control */
  if (frame->window_update.window_size_increment == 0) {
    return session_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO,
        "WINDOW_UPDATE: window_size_increment == 0");
  }

  if (NGHTTP2_MAX_WINDOW_SIZE - frame->window_update.window_size_increment <
      session->remote_window_size) {
    return session_handle_invalid_connection(session, frame,
                                             NGHTTP2_ERR_FLOW_CONTROL, NULL);
  }
  session->remote_window_size += frame->window_update.window_size_increment;

  return session_call_on_frame_received(session, frame);
}