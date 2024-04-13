int nghttp2_session_want_write(nghttp2_session *session) {
  /* If these flag is set, we don't want to write any data. The
     application should drop the connection. */
  if (session->goaway_flags & NGHTTP2_GOAWAY_TERM_SENT) {
    return 0;
  }

  /*
   * Unless termination GOAWAY is sent or received, we want to write
   * frames if there is pending ones. If pending frame is request/push
   * response HEADERS and concurrent stream limit is reached, we don't
   * want to write them.
   */
  return session->aob.item || nghttp2_outbound_queue_top(&session->ob_urgent) ||
         nghttp2_outbound_queue_top(&session->ob_reg) ||
         (!nghttp2_pq_empty(&session->root.obq) &&
          session->remote_window_size > 0) ||
         (nghttp2_outbound_queue_top(&session->ob_syn) &&
          !session_is_outgoing_concurrent_streams_max(session));
}