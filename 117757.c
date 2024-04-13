int nghttp2_session_create_idle_stream(nghttp2_session *session,
                                       int32_t stream_id,
                                       const nghttp2_priority_spec *pri_spec) {
  nghttp2_stream *stream;
  nghttp2_priority_spec pri_spec_copy;

  if (stream_id == 0 || stream_id == pri_spec->stream_id ||
      !session_detect_idle_stream(session, stream_id)) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  stream = nghttp2_session_get_stream_raw(session, stream_id);
  if (stream) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  pri_spec_copy = *pri_spec;
  nghttp2_priority_spec_normalize_weight(&pri_spec_copy);

  stream =
      nghttp2_session_open_stream(session, stream_id, NGHTTP2_STREAM_FLAG_NONE,
                                  &pri_spec_copy, NGHTTP2_STREAM_IDLE, NULL);
  if (!stream) {
    return NGHTTP2_ERR_NOMEM;
  }

  /* We don't intentionally call nghttp2_session_adjust_idle_stream()
     so that idle stream created by this function, and existing ones
     are kept for application.  We will adjust number of idle stream
     in nghttp2_session_mem_send or nghttp2_session_mem_recv is
     called. */
  return 0;
}