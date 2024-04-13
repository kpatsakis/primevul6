static int session_on_stream_window_update_received(nghttp2_session *session,
                                                    nghttp2_frame *frame) {
  int rv;
  nghttp2_stream *stream;

  if (session_detect_idle_stream(session, frame->hd.stream_id)) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "WINDOW_UPDATE to idle stream");
  }

  stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
  if (!stream) {
    return 0;
  }
  if (state_reserved_remote(session, stream)) {
    return session_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO, "WINDOW_UPADATE to reserved stream");
  }
  if (frame->window_update.window_size_increment == 0) {
    return session_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO,
        "WINDOW_UPDATE: window_size_increment == 0");
  }
  if (NGHTTP2_MAX_WINDOW_SIZE - frame->window_update.window_size_increment <
      stream->remote_window_size) {
    return session_handle_invalid_stream(session, frame,
                                         NGHTTP2_ERR_FLOW_CONTROL);
  }
  stream->remote_window_size += frame->window_update.window_size_increment;

  if (stream->remote_window_size > 0 &&
      nghttp2_stream_check_deferred_by_flow_control(stream)) {

    rv = nghttp2_stream_resume_deferred_item(
        stream, NGHTTP2_STREAM_FLAG_DEFERRED_FLOW_CONTROL);

    if (nghttp2_is_fatal(rv)) {
      return rv;
    }
  }
  return session_call_on_frame_received(session, frame);
}