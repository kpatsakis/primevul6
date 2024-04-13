int nghttp2_session_update_local_settings(nghttp2_session *session,
                                          nghttp2_settings_entry *iv,
                                          size_t niv) {
  int rv;
  size_t i;
  int32_t new_initial_window_size = -1;
  uint32_t header_table_size = 0;
  uint32_t min_header_table_size = UINT32_MAX;
  uint8_t header_table_size_seen = 0;
  /* For NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE, use the value last
     seen.  For NGHTTP2_SETTINGS_HEADER_TABLE_SIZE, use both minimum
     value and last seen value. */
  for (i = 0; i < niv; ++i) {
    switch (iv[i].settings_id) {
    case NGHTTP2_SETTINGS_HEADER_TABLE_SIZE:
      header_table_size_seen = 1;
      header_table_size = iv[i].value;
      min_header_table_size = nghttp2_min(min_header_table_size, iv[i].value);
      break;
    case NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE:
      new_initial_window_size = (int32_t)iv[i].value;
      break;
    }
  }
  if (header_table_size_seen) {
    if (min_header_table_size < header_table_size) {
      rv = nghttp2_hd_inflate_change_table_size(&session->hd_inflater,
                                                min_header_table_size);
      if (rv != 0) {
        return rv;
      }
    }

    rv = nghttp2_hd_inflate_change_table_size(&session->hd_inflater,
                                              header_table_size);
    if (rv != 0) {
      return rv;
    }
  }
  if (new_initial_window_size != -1) {
    rv = session_update_local_initial_window_size(
        session, new_initial_window_size,
        (int32_t)session->local_settings.initial_window_size);
    if (rv != 0) {
      return rv;
    }
  }

  for (i = 0; i < niv; ++i) {
    switch (iv[i].settings_id) {
    case NGHTTP2_SETTINGS_HEADER_TABLE_SIZE:
      session->local_settings.header_table_size = iv[i].value;
      break;
    case NGHTTP2_SETTINGS_ENABLE_PUSH:
      session->local_settings.enable_push = iv[i].value;
      break;
    case NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS:
      session->local_settings.max_concurrent_streams = iv[i].value;
      break;
    case NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE:
      session->local_settings.initial_window_size = iv[i].value;
      break;
    case NGHTTP2_SETTINGS_MAX_FRAME_SIZE:
      session->local_settings.max_frame_size = iv[i].value;
      break;
    case NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE:
      session->local_settings.max_header_list_size = iv[i].value;
      break;
    case NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL:
      session->local_settings.enable_connect_protocol = iv[i].value;
      break;
    }
  }

  return 0;
}