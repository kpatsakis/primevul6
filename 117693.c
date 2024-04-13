int nghttp2_session_on_rst_stream_received(nghttp2_session *session,
                                           nghttp2_frame *frame) {
  int rv;
  nghttp2_stream *stream;
  if (frame->hd.stream_id == 0) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "RST_STREAM: stream_id == 0");
  }

  if (session_detect_idle_stream(session, frame->hd.stream_id)) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "RST_STREAM: stream in idle");
  }

  stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
  if (stream) {
    /* We may use stream->shut_flags for strict error checking. */
    nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_RD);
  }

  rv = session_call_on_frame_received(session, frame);
  if (rv != 0) {
    return rv;
  }
  rv = nghttp2_session_close_stream(session, frame->hd.stream_id,
                                    frame->rst_stream.error_code);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }
  return 0;
}