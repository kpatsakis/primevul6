void nghttp2_session_keep_closed_stream(nghttp2_session *session,
                                        nghttp2_stream *stream) {
  DEBUGF("stream: keep closed stream(%p)=%d, state=%d\n", stream,
         stream->stream_id, stream->state);

  if (session->closed_stream_tail) {
    session->closed_stream_tail->closed_next = stream;
    stream->closed_prev = session->closed_stream_tail;
  } else {
    session->closed_stream_head = stream;
  }
  session->closed_stream_tail = stream;

  ++session->num_closed_streams;
}