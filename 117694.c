nghttp2_session_get_effective_recv_data_length(nghttp2_session *session) {
  return session->recv_window_size < 0 ? 0 : session->recv_window_size;
}