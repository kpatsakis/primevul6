static int session_update_stream_consumed_size(nghttp2_session *session,
                                               nghttp2_stream *stream,
                                               size_t delta_size) {
  return session_update_consumed_size(
      session, &stream->consumed_size, &stream->recv_window_size,
      stream->window_update_queued, stream->stream_id, delta_size,
      stream->local_window_size);
}