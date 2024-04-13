int nghttp2_session_is_my_stream_id(nghttp2_session *session,
                                    int32_t stream_id) {
  int rem;
  if (stream_id == 0) {
    return 0;
  }
  rem = stream_id & 0x1;
  if (session->server) {
    return rem == 0;
  }
  return rem == 1;
}