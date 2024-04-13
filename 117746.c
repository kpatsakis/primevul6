nghttp2_stream *nghttp2_session_find_stream(nghttp2_session *session,
                                            int32_t stream_id) {
  if (stream_id == 0) {
    return &session->root;
  }

  return nghttp2_session_get_stream_raw(session, stream_id);
}