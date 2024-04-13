int nghttp2_session_add_goaway(nghttp2_session *session, int32_t last_stream_id,
                               uint32_t error_code, const uint8_t *opaque_data,
                               size_t opaque_data_len, uint8_t aux_flags) {
  int rv;
  nghttp2_outbound_item *item;
  nghttp2_frame *frame;
  uint8_t *opaque_data_copy = NULL;
  nghttp2_goaway_aux_data *aux_data;
  nghttp2_mem *mem;

  mem = &session->mem;

  if (nghttp2_session_is_my_stream_id(session, last_stream_id)) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  if (opaque_data_len) {
    if (opaque_data_len + 8 > NGHTTP2_MAX_PAYLOADLEN) {
      return NGHTTP2_ERR_INVALID_ARGUMENT;
    }
    opaque_data_copy = nghttp2_mem_malloc(mem, opaque_data_len);
    if (opaque_data_copy == NULL) {
      return NGHTTP2_ERR_NOMEM;
    }
    memcpy(opaque_data_copy, opaque_data, opaque_data_len);
  }

  item = nghttp2_mem_malloc(mem, sizeof(nghttp2_outbound_item));
  if (item == NULL) {
    nghttp2_mem_free(mem, opaque_data_copy);
    return NGHTTP2_ERR_NOMEM;
  }

  nghttp2_outbound_item_init(item);

  frame = &item->frame;

  /* last_stream_id must not be increased from the value previously
     sent */
  last_stream_id = nghttp2_min(last_stream_id, session->local_last_stream_id);

  nghttp2_frame_goaway_init(&frame->goaway, last_stream_id, error_code,
                            opaque_data_copy, opaque_data_len);

  aux_data = &item->aux_data.goaway;
  aux_data->flags = aux_flags;

  rv = nghttp2_session_add_item(session, item);
  if (rv != 0) {
    nghttp2_frame_goaway_free(&frame->goaway, mem);
    nghttp2_mem_free(mem, item);
    return rv;
  }
  return 0;
}