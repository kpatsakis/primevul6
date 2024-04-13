size_t nghttp2_session_get_outbound_queue_size(nghttp2_session *session) {
  return nghttp2_outbound_queue_size(&session->ob_urgent) +
         nghttp2_outbound_queue_size(&session->ob_reg) +
         nghttp2_outbound_queue_size(&session->ob_syn);
  /* TODO account for item attached to stream */
}