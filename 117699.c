int nghttp2_session_server_new2(nghttp2_session **session_ptr,
                                const nghttp2_session_callbacks *callbacks,
                                void *user_data, const nghttp2_option *option) {
  return nghttp2_session_server_new3(session_ptr, callbacks, user_data, option,
                                     NULL);
}