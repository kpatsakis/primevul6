session_is_incoming_concurrent_streams_max(nghttp2_session *session) {
  return session->local_settings.max_concurrent_streams <=
         session->num_incoming_streams;
}