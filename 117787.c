int nghttp2_session_terminate_session(nghttp2_session *session,
                                      uint32_t error_code) {
  return session_terminate_session(session, session->last_proc_stream_id,
                                   error_code, NULL);
}