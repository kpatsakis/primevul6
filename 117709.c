int nghttp2_session_on_settings_received(nghttp2_session *session,
                                         nghttp2_frame *frame, int noack) {
  int rv;
  size_t i;
  nghttp2_mem *mem;
  nghttp2_inflight_settings *settings;

  mem = &session->mem;

  if (frame->hd.stream_id != 0) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "SETTINGS: stream_id != 0");
  }
  if (frame->hd.flags & NGHTTP2_FLAG_ACK) {
    if (frame->settings.niv != 0) {
      return session_handle_invalid_connection(
          session, frame, NGHTTP2_ERR_FRAME_SIZE_ERROR,
          "SETTINGS: ACK and payload != 0");
    }

    settings = session->inflight_settings_head;

    if (!settings) {
      return session_handle_invalid_connection(
          session, frame, NGHTTP2_ERR_PROTO, "SETTINGS: unexpected ACK");
    }

    rv = nghttp2_session_update_local_settings(session, settings->iv,
                                               settings->niv);

    session->inflight_settings_head = settings->next;

    inflight_settings_del(settings, mem);

    if (rv != 0) {
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
      return session_handle_invalid_connection(session, frame, rv, NULL);
    }
    return session_call_on_frame_received(session, frame);
  }

  if (!session->remote_settings_received) {
    session->remote_settings.max_concurrent_streams =
        NGHTTP2_DEFAULT_MAX_CONCURRENT_STREAMS;
    session->remote_settings_received = 1;
  }

  for (i = 0; i < frame->settings.niv; ++i) {
    nghttp2_settings_entry *entry = &frame->settings.iv[i];

    switch (entry->settings_id) {
    case NGHTTP2_SETTINGS_HEADER_TABLE_SIZE:

      rv = nghttp2_hd_deflate_change_table_size(&session->hd_deflater,
                                                entry->value);
      if (rv != 0) {
        if (nghttp2_is_fatal(rv)) {
          return rv;
        } else {
          return session_handle_invalid_connection(
              session, frame, NGHTTP2_ERR_HEADER_COMP, NULL);
        }
      }

      session->remote_settings.header_table_size = entry->value;

      break;
    case NGHTTP2_SETTINGS_ENABLE_PUSH:

      if (entry->value != 0 && entry->value != 1) {
        return session_handle_invalid_connection(
            session, frame, NGHTTP2_ERR_PROTO,
            "SETTINGS: invalid SETTINGS_ENBLE_PUSH");
      }

      if (!session->server && entry->value != 0) {
        return session_handle_invalid_connection(
            session, frame, NGHTTP2_ERR_PROTO,
            "SETTINGS: server attempted to enable push");
      }

      session->remote_settings.enable_push = entry->value;

      break;
    case NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS:

      session->remote_settings.max_concurrent_streams = entry->value;

      break;
    case NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE:

      /* Update the initial window size of the all active streams */
      /* Check that initial_window_size < (1u << 31) */
      if (entry->value > NGHTTP2_MAX_WINDOW_SIZE) {
        return session_handle_invalid_connection(
            session, frame, NGHTTP2_ERR_FLOW_CONTROL,
            "SETTINGS: too large SETTINGS_INITIAL_WINDOW_SIZE");
      }

      rv = session_update_remote_initial_window_size(session,
                                                     (int32_t)entry->value);

      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      if (rv != 0) {
        return session_handle_invalid_connection(
            session, frame, NGHTTP2_ERR_FLOW_CONTROL, NULL);
      }

      session->remote_settings.initial_window_size = entry->value;

      break;
    case NGHTTP2_SETTINGS_MAX_FRAME_SIZE:

      if (entry->value < NGHTTP2_MAX_FRAME_SIZE_MIN ||
          entry->value > NGHTTP2_MAX_FRAME_SIZE_MAX) {
        return session_handle_invalid_connection(
            session, frame, NGHTTP2_ERR_PROTO,
            "SETTINGS: invalid SETTINGS_MAX_FRAME_SIZE");
      }

      session->remote_settings.max_frame_size = entry->value;

      break;
    case NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE:

      session->remote_settings.max_header_list_size = entry->value;

      break;
    case NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL:

      if (entry->value != 0 && entry->value != 1) {
        return session_handle_invalid_connection(
            session, frame, NGHTTP2_ERR_PROTO,
            "SETTINGS: invalid SETTINGS_ENABLE_CONNECT_PROTOCOL");
      }

      if (!session->server &&
          session->remote_settings.enable_connect_protocol &&
          entry->value == 0) {
        return session_handle_invalid_connection(
            session, frame, NGHTTP2_ERR_PROTO,
            "SETTINGS: server attempted to disable "
            "SETTINGS_ENABLE_CONNECT_PROTOCOL");
      }

      session->remote_settings.enable_connect_protocol = entry->value;

      break;
    }
  }

  if (!noack && !session_is_closing(session)) {
    rv = nghttp2_session_add_settings(session, NGHTTP2_FLAG_ACK, NULL, 0);

    if (rv != 0) {
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      return session_handle_invalid_connection(session, frame,
                                               NGHTTP2_ERR_INTERNAL, NULL);
    }
  }

  return session_call_on_frame_received(session, frame);
}