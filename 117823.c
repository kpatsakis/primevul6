nghttp2_stream *nghttp2_session_get_stream_raw(nghttp2_session *session,
                                               int32_t stream_id) {
  return (nghttp2_stream *)nghttp2_map_find(&session->streams, stream_id);
}