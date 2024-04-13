session_is_outgoing_concurrent_streams_max(nghttp2_session *session) {
  return session->remote_settings.max_concurrent_streams <=
         session->num_outgoing_streams;
}