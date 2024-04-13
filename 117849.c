static int session_close_stream_on_goaway(nghttp2_session *session,
                                          int32_t last_stream_id,
                                          int incoming) {
  int rv;
  nghttp2_stream *stream, *next_stream;
  nghttp2_close_stream_on_goaway_arg arg = {session, NULL, last_stream_id,
                                            incoming};

  rv = nghttp2_map_each(&session->streams, find_stream_on_goaway_func, &arg);
  assert(rv == 0);

  stream = arg.head;
  while (stream) {
    next_stream = stream->closed_next;
    stream->closed_next = NULL;
    rv = nghttp2_session_close_stream(session, stream->stream_id,
                                      NGHTTP2_REFUSED_STREAM);

    /* stream may be deleted here */

    stream = next_stream;

    if (nghttp2_is_fatal(rv)) {
      /* Clean up closed_next member just in case */
      while (stream) {
        next_stream = stream->closed_next;
        stream->closed_next = NULL;
        stream = next_stream;
      }
      return rv;
    }
  }

  return 0;
}