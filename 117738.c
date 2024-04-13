void nghttp2_session_detach_idle_stream(nghttp2_session *session,
                                        nghttp2_stream *stream) {
  nghttp2_stream *prev_stream, *next_stream;

  DEBUGF("stream: detach idle stream(%p)=%d, state=%d\n", stream,
         stream->stream_id, stream->state);

  prev_stream = stream->closed_prev;
  next_stream = stream->closed_next;

  if (prev_stream) {
    prev_stream->closed_next = next_stream;
  } else {
    session->idle_stream_head = next_stream;
  }

  if (next_stream) {
    next_stream->closed_prev = prev_stream;
  } else {
    session->idle_stream_tail = prev_stream;
  }

  stream->closed_prev = NULL;
  stream->closed_next = NULL;

  --session->num_idle_streams;
}