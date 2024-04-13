static int session_is_new_peer_stream_id(nghttp2_session *session,
                                         int32_t stream_id) {
  return stream_id != 0 &&
         !nghttp2_session_is_my_stream_id(session, stream_id) &&
         session->last_recv_stream_id < stream_id;
}