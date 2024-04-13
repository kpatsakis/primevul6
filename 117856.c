static int session_process_headers_frame(nghttp2_session *session) {
  int rv;
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_frame *frame = &iframe->frame;
  nghttp2_stream *stream;

  rv = nghttp2_frame_unpack_headers_payload(&frame->headers, iframe->sbuf.pos);

  if (rv != 0) {
    return nghttp2_session_terminate_session_with_reason(
        session, NGHTTP2_PROTOCOL_ERROR, "HEADERS: could not unpack");
  }
  stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
  if (!stream) {
    frame->headers.cat = NGHTTP2_HCAT_REQUEST;
    return nghttp2_session_on_request_headers_received(session, frame);
  }

  if (stream->state == NGHTTP2_STREAM_RESERVED) {
    frame->headers.cat = NGHTTP2_HCAT_PUSH_RESPONSE;
    return nghttp2_session_on_push_response_headers_received(session, frame,
                                                             stream);
  }

  if (stream->state == NGHTTP2_STREAM_OPENING &&
      nghttp2_session_is_my_stream_id(session, frame->hd.stream_id)) {
    frame->headers.cat = NGHTTP2_HCAT_RESPONSE;
    return nghttp2_session_on_response_headers_received(session, frame, stream);
  }

  frame->headers.cat = NGHTTP2_HCAT_HEADERS;
  return nghttp2_session_on_headers_received(session, frame, stream);
}