static void inbound_frame_set_settings_entry(nghttp2_inbound_frame *iframe) {
  nghttp2_settings_entry iv;
  nghttp2_settings_entry *min_header_table_size_entry;
  size_t i;

  nghttp2_frame_unpack_settings_entry(&iv, iframe->sbuf.pos);

  switch (iv.settings_id) {
  case NGHTTP2_SETTINGS_HEADER_TABLE_SIZE:
  case NGHTTP2_SETTINGS_ENABLE_PUSH:
  case NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS:
  case NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE:
  case NGHTTP2_SETTINGS_MAX_FRAME_SIZE:
  case NGHTTP2_SETTINGS_MAX_HEADER_LIST_SIZE:
  case NGHTTP2_SETTINGS_ENABLE_CONNECT_PROTOCOL:
    break;
  default:
    DEBUGF("recv: unknown settings id=0x%02x\n", iv.settings_id);

    iframe->iv[iframe->niv++] = iv;

    return;
  }

  for (i = 0; i < iframe->niv; ++i) {
    if (iframe->iv[i].settings_id == iv.settings_id) {
      iframe->iv[i] = iv;
      break;
    }
  }

  if (i == iframe->niv) {
    iframe->iv[iframe->niv++] = iv;
  }

  if (iv.settings_id == NGHTTP2_SETTINGS_HEADER_TABLE_SIZE) {
    /* Keep track of minimum value of SETTINGS_HEADER_TABLE_SIZE */
    min_header_table_size_entry = &iframe->iv[iframe->max_niv - 1];

    if (iv.value < min_header_table_size_entry->value) {
      min_header_table_size_entry->value = iv.value;
    }
  }
}