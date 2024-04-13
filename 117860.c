int nghttp2_session_set_next_stream_id(nghttp2_session *session,
                                       int32_t next_stream_id) {
  if (next_stream_id <= 0 ||
      session->next_stream_id > (uint32_t)next_stream_id) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  if (session->server) {
    if (next_stream_id % 2) {
      return NGHTTP2_ERR_INVALID_ARGUMENT;
    }
  } else if (next_stream_id % 2 == 0) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  session->next_stream_id = (uint32_t)next_stream_id;
  return 0;
}