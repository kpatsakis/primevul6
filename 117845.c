int nghttp2_session_server_new(nghttp2_session **session_ptr,
                               const nghttp2_session_callbacks *callbacks,
                               void *user_data) {
  return nghttp2_session_server_new3(session_ptr, callbacks, user_data, NULL,
                                     NULL);
}