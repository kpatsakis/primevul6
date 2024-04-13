int nghttp2_session_on_push_response_headers_received(nghttp2_session *session,
                                                      nghttp2_frame *frame,
                                                      nghttp2_stream *stream) {
  int rv = 0;
  assert(stream->state == NGHTTP2_STREAM_RESERVED);
  if (frame->hd.stream_id == 0) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO,
        "push response HEADERS: stream_id == 0");
  }

  if (session->server) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO,
        "HEADERS: no HEADERS allowed from client in reserved state");
  }

  if (session_is_incoming_concurrent_streams_max(session)) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO,
        "push response HEADERS: max concurrent streams exceeded");
  }

  if (!session_allow_incoming_new_stream(session)) {
    /* We don't accept new stream after GOAWAY was sent. */
    return NGHTTP2_ERR_IGN_HEADER_BLOCK;
  }

  if (session_is_incoming_concurrent_streams_pending_max(session)) {
    return session_inflate_handle_invalid_stream(session, frame,
                                                 NGHTTP2_ERR_REFUSED_STREAM);
  }

  nghttp2_stream_promise_fulfilled(stream);
  if (!nghttp2_session_is_my_stream_id(session, stream->stream_id)) {
    --session->num_incoming_reserved_streams;
  }
  ++session->num_incoming_streams;
  rv = session_call_on_begin_headers(session, frame);
  if (rv != 0) {
    return rv;
  }
  return 0;
}