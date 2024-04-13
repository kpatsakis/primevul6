int nghttp2_session_adjust_closed_stream(nghttp2_session *session) {
  size_t num_stream_max;
  int rv;

  if (session->local_settings.max_concurrent_streams ==
      NGHTTP2_DEFAULT_MAX_CONCURRENT_STREAMS) {
    num_stream_max = session->pending_local_max_concurrent_stream;
  } else {
    num_stream_max = session->local_settings.max_concurrent_streams;
  }

  DEBUGF("stream: adjusting kept closed streams num_closed_streams=%zu, "
         "num_incoming_streams=%zu, max_concurrent_streams=%zu\n",
         session->num_closed_streams, session->num_incoming_streams,
         num_stream_max);

  while (session->num_closed_streams > 0 &&
         session->num_closed_streams + session->num_incoming_streams >
             num_stream_max) {
    nghttp2_stream *head_stream;
    nghttp2_stream *next;

    head_stream = session->closed_stream_head;

    assert(head_stream);

    next = head_stream->closed_next;

    rv = nghttp2_session_destroy_stream(session, head_stream);
    if (rv != 0) {
      return rv;
    }

    /* head_stream is now freed */

    session->closed_stream_head = next;

    if (session->closed_stream_head) {
      session->closed_stream_head->closed_prev = NULL;
    } else {
      session->closed_stream_tail = NULL;
    }

    --session->num_closed_streams;
  }

  return 0;
}