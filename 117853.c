int nghttp2_session_on_priority_received(nghttp2_session *session,
                                         nghttp2_frame *frame) {
  int rv;
  nghttp2_stream *stream;

  if (frame->hd.stream_id == 0) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "PRIORITY: stream_id == 0");
  }

  if (frame->priority.pri_spec.stream_id == frame->hd.stream_id) {
    return nghttp2_session_terminate_session_with_reason(
        session, NGHTTP2_PROTOCOL_ERROR, "depend on itself");
  }

  if (!session->server) {
    /* Re-prioritization works only in server */
    return session_call_on_frame_received(session, frame);
  }

  stream = nghttp2_session_get_stream_raw(session, frame->hd.stream_id);

  if (!stream) {
    /* PRIORITY against idle stream can create anchor node in
       dependency tree. */
    if (!session_detect_idle_stream(session, frame->hd.stream_id)) {
      return 0;
    }

    stream = nghttp2_session_open_stream(
        session, frame->hd.stream_id, NGHTTP2_STREAM_FLAG_NONE,
        &frame->priority.pri_spec, NGHTTP2_STREAM_IDLE, NULL);

    if (stream == NULL) {
      return NGHTTP2_ERR_NOMEM;
    }

    rv = nghttp2_session_adjust_idle_stream(session);
    if (nghttp2_is_fatal(rv)) {
      return rv;
    }
  } else {
    rv = nghttp2_session_reprioritize_stream(session, stream,
                                             &frame->priority.pri_spec);

    if (nghttp2_is_fatal(rv)) {
      return rv;
    }

    rv = nghttp2_session_adjust_idle_stream(session);
    if (nghttp2_is_fatal(rv)) {
      return rv;
    }
  }

  return session_call_on_frame_received(session, frame);
}