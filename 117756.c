int nghttp2_session_on_request_headers_received(nghttp2_session *session,
                                                nghttp2_frame *frame) {
  int rv = 0;
  nghttp2_stream *stream;
  if (frame->hd.stream_id == 0) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO, "request HEADERS: stream_id == 0");
  }

  /* If client receives idle stream from server, it is invalid
     regardless stream ID is even or odd.  This is because client is
     not expected to receive request from server. */
  if (!session->server) {
    if (session_detect_idle_stream(session, frame->hd.stream_id)) {
      return session_inflate_handle_invalid_connection(
          session, frame, NGHTTP2_ERR_PROTO,
          "request HEADERS: client received request");
    }

    return NGHTTP2_ERR_IGN_HEADER_BLOCK;
  }

  assert(session->server);

  if (!session_is_new_peer_stream_id(session, frame->hd.stream_id)) {
    if (frame->hd.stream_id == 0 ||
        nghttp2_session_is_my_stream_id(session, frame->hd.stream_id)) {
      return session_inflate_handle_invalid_connection(
          session, frame, NGHTTP2_ERR_PROTO,
          "request HEADERS: invalid stream_id");
    }

    /* RFC 7540 says if an endpoint receives a HEADERS with invalid
     * stream ID (e.g, numerically smaller than previous), it MUST
     * issue connection error with error code PROTOCOL_ERROR.  It is a
     * bit hard to detect this, since we cannot remember all streams
     * we observed so far.
     *
     * You might imagine this is really easy.  But no.  HTTP/2 is
     * asynchronous protocol, and usually client and server do not
     * share the complete picture of open/closed stream status.  For
     * example, after server sends RST_STREAM for a stream, client may
     * send trailer HEADERS for that stream.  If naive server detects
     * that, and issued connection error, then it is a bug of server
     * implementation since client is not wrong if it did not get
     * RST_STREAM when it issued trailer HEADERS.
     *
     * At the moment, we are very conservative here.  We only use
     * connection error if stream ID refers idle stream, or we are
     * sure that stream is half-closed(remote) or closed.  Otherwise
     * we just ignore HEADERS for now.
     */
    stream = nghttp2_session_get_stream_raw(session, frame->hd.stream_id);
    if (stream && (stream->shut_flags & NGHTTP2_SHUT_RD)) {
      return session_inflate_handle_invalid_connection(
          session, frame, NGHTTP2_ERR_STREAM_CLOSED, "HEADERS: stream closed");
    }

    return NGHTTP2_ERR_IGN_HEADER_BLOCK;
  }
  session->last_recv_stream_id = frame->hd.stream_id;

  if (session_is_incoming_concurrent_streams_max(session)) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO,
        "request HEADERS: max concurrent streams exceeded");
  }

  if (!session_allow_incoming_new_stream(session)) {
    /* We just ignore stream after GOAWAY was sent */
    return NGHTTP2_ERR_IGN_HEADER_BLOCK;
  }

  if (frame->headers.pri_spec.stream_id == frame->hd.stream_id) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO, "request HEADERS: depend on itself");
  }

  if (session_is_incoming_concurrent_streams_pending_max(session)) {
    return session_inflate_handle_invalid_stream(session, frame,
                                                 NGHTTP2_ERR_REFUSED_STREAM);
  }

  stream = nghttp2_session_open_stream(
      session, frame->hd.stream_id, NGHTTP2_STREAM_FLAG_NONE,
      &frame->headers.pri_spec, NGHTTP2_STREAM_OPENING, NULL);
  if (!stream) {
    return NGHTTP2_ERR_NOMEM;
  }

  rv = nghttp2_session_adjust_closed_stream(session);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  session->last_proc_stream_id = session->last_recv_stream_id;

  rv = session_call_on_begin_headers(session, frame);
  if (rv != 0) {
    return rv;
  }
  return 0;
}