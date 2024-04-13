static int session_on_data_received_fail_fast(nghttp2_session *session) {
  int rv;
  nghttp2_stream *stream;
  nghttp2_inbound_frame *iframe;
  int32_t stream_id;
  const char *failure_reason;
  uint32_t error_code = NGHTTP2_PROTOCOL_ERROR;

  iframe = &session->iframe;
  stream_id = iframe->frame.hd.stream_id;

  if (stream_id == 0) {
    /* The spec says that if a DATA frame is received whose stream ID
       is 0, the recipient MUST respond with a connection error of
       type PROTOCOL_ERROR. */
    failure_reason = "DATA: stream_id == 0";
    goto fail;
  }

  if (session_detect_idle_stream(session, stream_id)) {
    failure_reason = "DATA: stream in idle";
    error_code = NGHTTP2_PROTOCOL_ERROR;
    goto fail;
  }

  stream = nghttp2_session_get_stream(session, stream_id);
  if (!stream) {
    stream = nghttp2_session_get_stream_raw(session, stream_id);
    if (stream && (stream->shut_flags & NGHTTP2_SHUT_RD)) {
      failure_reason = "DATA: stream closed";
      error_code = NGHTTP2_STREAM_CLOSED;
      goto fail;
    }

    return NGHTTP2_ERR_IGN_PAYLOAD;
  }
  if (stream->shut_flags & NGHTTP2_SHUT_RD) {
    failure_reason = "DATA: stream in half-closed(remote)";
    error_code = NGHTTP2_STREAM_CLOSED;
    goto fail;
  }

  if (nghttp2_session_is_my_stream_id(session, stream_id)) {
    if (stream->state == NGHTTP2_STREAM_CLOSING) {
      return NGHTTP2_ERR_IGN_PAYLOAD;
    }
    if (stream->state != NGHTTP2_STREAM_OPENED) {
      failure_reason = "DATA: stream not opened";
      goto fail;
    }
    return 0;
  }
  if (stream->state == NGHTTP2_STREAM_RESERVED) {
    failure_reason = "DATA: stream in reserved";
    goto fail;
  }
  if (stream->state == NGHTTP2_STREAM_CLOSING) {
    return NGHTTP2_ERR_IGN_PAYLOAD;
  }
  return 0;
fail:
  rv = nghttp2_session_terminate_session_with_reason(session, error_code,
                                                     failure_reason);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }
  return NGHTTP2_ERR_IGN_PAYLOAD;
}