uint32_t nghttp2_session_get_remote_settings(nghttp2_session *session,
                                             nghttp2_settings_id id) {
  switch (id) {
  case NGHTTP2_SETTINGS_HEADER_TABLE_SIZE:
    return session->remote_settings.header_table_size;
  case NGHTTP2_SETTINGS_ENABLE_PUSH:
    return session->remote_settings.enable_push;
  case NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS:
    return session->remote_settings.max_concurrent_streams;
  case NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE:
    return session->remote_settings.initial_window_size;
  case NGHTTP2_SETTINGS_MAX_FRAME_SIZE:
    return session->remote_settings.max_frame_size;
  case NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE:
    return session->remote_settings.max_header_list_size;
  case NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL:
    return session->remote_settings.enable_connect_protocol;
  }

  assert(0);
  abort(); /* if NDEBUG is set */
}