static int find_stream_on_goaway_func(nghttp2_map_entry *entry, void *ptr) {
  nghttp2_close_stream_on_goaway_arg *arg;
  nghttp2_stream *stream;

  arg = (nghttp2_close_stream_on_goaway_arg *)ptr;
  stream = (nghttp2_stream *)entry;

  if (nghttp2_session_is_my_stream_id(arg->session, stream->stream_id)) {
    if (arg->incoming) {
      return 0;
    }
  } else if (!arg->incoming) {
    return 0;
  }

  if (stream->state != NGHTTP2_STREAM_IDLE &&
      (stream->flags & NGHTTP2_STREAM_FLAG_CLOSED) == 0 &&
      stream->stream_id > arg->last_stream_id) {
    /* We are collecting streams to close because we cannot call
       nghttp2_session_close_stream() inside nghttp2_map_each().
       Reuse closed_next member.. bad choice? */
    assert(stream->closed_next == NULL);
    assert(stream->closed_prev == NULL);

    if (arg->head) {
      stream->closed_next = arg->head;
      arg->head = stream;
    } else {
      arg->head = stream;
    }
  }

  return 0;
}