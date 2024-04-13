void nghttp2_session_keep_idle_stream(nghttp2_session *session,
                                      nghttp2_stream *stream) {
  DEBUGF("stream: keep idle stream(%p)=%d, state=%d\n", stream,
         stream->stream_id, stream->state);

  if (session->idle_stream_tail) {
    session->idle_stream_tail->closed_next = stream;
    stream->closed_prev = session->idle_stream_tail;
  } else {
    session->idle_stream_head = stream;
  }
  session->idle_stream_tail = stream;

  ++session->num_idle_streams;
}