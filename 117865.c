int32_t nghttp2_session_get_last_proc_stream_id(nghttp2_session *session) {
  return session->last_proc_stream_id;
}