nghttp2_session_get_next_ob_item(nghttp2_session *session) {
  if (nghttp2_outbound_queue_top(&session->ob_urgent)) {
    return nghttp2_outbound_queue_top(&session->ob_urgent);
  }

  if (nghttp2_outbound_queue_top(&session->ob_reg)) {
    return nghttp2_outbound_queue_top(&session->ob_reg);
  }

  if (!session_is_outgoing_concurrent_streams_max(session)) {
    if (nghttp2_outbound_queue_top(&session->ob_syn)) {
      return nghttp2_outbound_queue_top(&session->ob_syn);
    }
  }

  if (session->remote_window_size > 0) {
    return nghttp2_stream_next_outbound_item(&session->root);
  }

  return NULL;
}