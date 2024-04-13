int nghttp2_session_adjust_idle_stream(nghttp2_session *session) {
  size_t max;
  int rv;

  /* Make minimum number of idle streams 16, and maximum 100, which
     are arbitrary chosen numbers. */
  max = nghttp2_min(
      100, nghttp2_max(
               16, nghttp2_min(session->local_settings.max_concurrent_streams,
                               session->pending_local_max_concurrent_stream)));

  DEBUGF("stream: adjusting kept idle streams num_idle_streams=%zu, max=%zu\n",
         session->num_idle_streams, max);

  while (session->num_idle_streams > max) {
    nghttp2_stream *head;
    nghttp2_stream *next;

    head = session->idle_stream_head;
    assert(head);

    next = head->closed_next;

    rv = nghttp2_session_destroy_stream(session, head);
    if (rv != 0) {
      return rv;
    }

    /* head is now destroyed */

    session->idle_stream_head = next;

    if (session->idle_stream_head) {
      session->idle_stream_head->closed_prev = NULL;
    } else {
      session->idle_stream_tail = NULL;
    }

    --session->num_idle_streams;
  }

  return 0;
}