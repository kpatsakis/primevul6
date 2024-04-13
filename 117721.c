int nghttp2_session_on_push_promise_received(nghttp2_session *session,
                                             nghttp2_frame *frame) {
  int rv;
  nghttp2_stream *stream;
  nghttp2_stream *promised_stream;
  nghttp2_priority_spec pri_spec;

  if (frame->hd.stream_id == 0) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO, "PUSH_PROMISE: stream_id == 0");
  }
  if (session->server || session->local_settings.enable_push == 0) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO, "PUSH_PROMISE: push disabled");
  }

  if (!nghttp2_session_is_my_stream_id(session, frame->hd.stream_id)) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO, "PUSH_PROMISE: invalid stream_id");
  }

  if (!session_allow_incoming_new_stream(session)) {
    /* We just discard PUSH_PROMISE after GOAWAY was sent */
    return NGHTTP2_ERR_IGN_HEADER_BLOCK;
  }

  if (!session_is_new_peer_stream_id(session,
                                     frame->push_promise.promised_stream_id)) {
    /* The spec says if an endpoint receives a PUSH_PROMISE with
       illegal stream ID is subject to a connection error of type
       PROTOCOL_ERROR. */
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO,
        "PUSH_PROMISE: invalid promised_stream_id");
  }

  if (session_detect_idle_stream(session, frame->hd.stream_id)) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_PROTO, "PUSH_PROMISE: stream in idle");
  }

  session->last_recv_stream_id = frame->push_promise.promised_stream_id;
  stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
  if (!stream || stream->state == NGHTTP2_STREAM_CLOSING ||
      !session->pending_enable_push ||
      session->num_incoming_reserved_streams >=
          session->max_incoming_reserved_streams) {
    /* Currently, client does not retain closed stream, so we don't
       check NGHTTP2_SHUT_RD condition here. */

    rv = nghttp2_session_add_rst_stream(
        session, frame->push_promise.promised_stream_id, NGHTTP2_CANCEL);
    if (rv != 0) {
      return rv;
    }
    return NGHTTP2_ERR_IGN_HEADER_BLOCK;
  }

  if (stream->shut_flags & NGHTTP2_SHUT_RD) {
    return session_inflate_handle_invalid_connection(
        session, frame, NGHTTP2_ERR_STREAM_CLOSED,
        "PUSH_PROMISE: stream closed");
  }

  nghttp2_priority_spec_init(&pri_spec, stream->stream_id,
                             NGHTTP2_DEFAULT_WEIGHT, 0);

  promised_stream = nghttp2_session_open_stream(
      session, frame->push_promise.promised_stream_id, NGHTTP2_STREAM_FLAG_NONE,
      &pri_spec, NGHTTP2_STREAM_RESERVED, NULL);

  if (!promised_stream) {
    return NGHTTP2_ERR_NOMEM;
  }

  /* We don't call nghttp2_session_adjust_closed_stream(), since we
     don't keep closed stream in client side */

  session->last_proc_stream_id = session->last_recv_stream_id;
  rv = session_call_on_begin_headers(session, frame);
  if (rv != 0) {
    return rv;
  }
  return 0;
}