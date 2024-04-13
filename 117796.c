int nghttp2_session_terminate_session_with_reason(nghttp2_session *session,
                                                  uint32_t error_code,
                                                  const char *reason) {
  return session_terminate_session(session, session->last_proc_stream_id,
                                   error_code, reason);
}