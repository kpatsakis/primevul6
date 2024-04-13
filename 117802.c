session_is_incoming_concurrent_streams_pending_max(nghttp2_session *session) {
  return session->pending_local_max_concurrent_stream <=
         session->num_incoming_streams;
}