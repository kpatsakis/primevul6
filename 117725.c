int nghttp2_session_add_window_update(nghttp2_session *session, uint8_t flags,
                                      int32_t stream_id,
                                      int32_t window_size_increment) {
  int rv;
  nghttp2_outbound_item *item;
  nghttp2_frame *frame;
  nghttp2_mem *mem;

  mem = &session->mem;
  item = nghttp2_mem_malloc(mem, sizeof(nghttp2_outbound_item));
  if (item == NULL) {
    return NGHTTP2_ERR_NOMEM;
  }

  nghttp2_outbound_item_init(item);

  frame = &item->frame;

  nghttp2_frame_window_update_init(&frame->window_update, flags, stream_id,
                                   window_size_increment);

  rv = nghttp2_session_add_item(session, item);

  if (rv != 0) {
    nghttp2_frame_window_update_free(&frame->window_update);
    nghttp2_mem_free(mem, item);
    return rv;
  }
  return 0;
}