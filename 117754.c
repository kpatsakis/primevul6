nghttp2_session_get_effective_local_window_size(nghttp2_session *session) {
  return session->local_window_size;
}