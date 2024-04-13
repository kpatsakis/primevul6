int nghttp2_session_client_new3(nghttp2_session **session_ptr,
                                const nghttp2_session_callbacks *callbacks,
                                void *user_data, const nghttp2_option *option,
                                nghttp2_mem *mem) {
  int rv;
  nghttp2_session *session;

  rv = session_new(&session, callbacks, user_data, 0, option, mem);

  if (rv != 0) {
    return rv;
  }
  /* IDs for use in client */
  session->next_stream_id = 1;

  *session_ptr = session;

  return 0;
}