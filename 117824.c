nghttp2_stream *nghttp2_session_get_stream(nghttp2_session *session,
                                           int32_t stream_id) {
  nghttp2_stream *stream;

  stream = (nghttp2_stream *)nghttp2_map_find(&session->streams, stream_id);

  if (stream == NULL || (stream->flags & NGHTTP2_STREAM_FLAG_CLOSED) ||
      stream->state == NGHTTP2_STREAM_IDLE) {
    return NULL;
  }

  return stream;
}