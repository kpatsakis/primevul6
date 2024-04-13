static int session_update_connection_consumed_size(nghttp2_session *session,
                                                   size_t delta_size) {
  return session_update_consumed_size(
      session, &session->consumed_size, &session->recv_window_size,
      session->window_update_queued, 0, delta_size, session->local_window_size);
}