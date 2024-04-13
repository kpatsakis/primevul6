int nghttp2_session_consume_stream(nghttp2_session *session, int32_t stream_id,
                                   size_t size) {
  int rv;
  nghttp2_stream *stream;

  if (stream_id == 0) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  if (!(session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_WINDOW_UPDATE)) {
    return NGHTTP2_ERR_INVALID_STATE;
  }

  stream = nghttp2_session_get_stream(session, stream_id);

  if (!stream) {
    return 0;
  }

  rv = session_update_stream_consumed_size(session, stream, size);

  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  return 0;
}