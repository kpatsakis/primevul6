int32_t nghttp2_session_get_remote_window_size(nghttp2_session *session) {
  return session->remote_window_size;
}