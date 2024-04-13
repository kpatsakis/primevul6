nghttp2_stream *nghttp2_session_get_root_stream(nghttp2_session *session) {
  return &session->root;
}