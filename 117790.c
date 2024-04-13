int nghttp2_session_resume_data(nghttp2_session *session, int32_t stream_id) {
  int rv;
  nghttp2_stream *stream;
  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream == NULL || !nghttp2_stream_check_deferred_item(stream)) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  rv = nghttp2_stream_resume_deferred_item(stream,
                                           NGHTTP2_STREAM_FLAG_DEFERRED_USER);

  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  return 0;
}