static int session_process_settings_frame(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;
  size_t i;
  nghttp2_settings_entry min_header_size_entry;

  if (iframe->max_niv) {
    min_header_size_entry = iframe->iv[iframe->max_niv - 1];

    if (min_header_size_entry.value < UINT32_MAX) {
      /* If we have less value, then we must have
         SETTINGS_HEADER_TABLE_SIZE in i < iframe->niv */
      for (i = 0; i < iframe->niv; ++i) {
        if (iframe->iv[i].settings_id == NGHTTP2_SETTINGS_HEADER_TABLE_SIZE) {
          break;
        }
      }

      assert(i < iframe->niv);

      if (min_header_size_entry.value != iframe->iv[i].value) {
        iframe->iv[iframe->niv++] = iframe->iv[i];
        iframe->iv[i] = min_header_size_entry;
      }
    }
  }

  nghttp2_frame_unpack_settings_payload(&frame->settings, iframe->iv,
                                        iframe->niv);

  iframe->iv = NULL;
  iframe->niv = 0;
  iframe->max_niv = 0;

  return nghttp2_session_on_settings_received(session, frame, 0 /* ACK */);
}