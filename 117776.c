uint32_t nghttp2_session_get_next_stream_id(nghttp2_session *session) {
  return session->next_stream_id;
}