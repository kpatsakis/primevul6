int32_t nghttp2_session_get_local_window_size(nghttp2_session *session) {
  return session->local_window_size - session->recv_window_size;
}