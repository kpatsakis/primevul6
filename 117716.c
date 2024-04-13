int nghttp2_session_terminate_session2(nghttp2_session *session,
                                       int32_t last_stream_id,
                                       uint32_t error_code) {
  return session_terminate_session(session, last_stream_id, error_code, NULL);
}