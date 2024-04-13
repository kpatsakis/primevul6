static int session_detect_idle_stream(nghttp2_session *session,
                                      int32_t stream_id) {
  /* Assume that stream object with stream_id does not exist */
  if (nghttp2_session_is_my_stream_id(session, stream_id)) {
    if (session->last_sent_stream_id < stream_id) {
      return 1;
    }
    return 0;
  }
  if (session_is_new_peer_stream_id(session, stream_id)) {
    return 1;
  }
  return 0;
}