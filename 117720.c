nghttp2_session_pop_next_ob_item(nghttp2_session *session) {
  nghttp2_outbound_item *item;

  item = nghttp2_outbound_queue_top(&session->ob_urgent);
  if (item) {
    nghttp2_outbound_queue_pop(&session->ob_urgent);
    item->queued = 0;
    return item;
  }

  item = nghttp2_outbound_queue_top(&session->ob_reg);
  if (item) {
    nghttp2_outbound_queue_pop(&session->ob_reg);
    item->queued = 0;
    return item;
  }

  if (!session_is_outgoing_concurrent_streams_max(session)) {
    item = nghttp2_outbound_queue_top(&session->ob_syn);
    if (item) {
      nghttp2_outbound_queue_pop(&session->ob_syn);
      item->queued = 0;
      return item;
    }
  }

  if (session->remote_window_size > 0) {
    return nghttp2_stream_next_outbound_item(&session->root);
  }

  return NULL;
}