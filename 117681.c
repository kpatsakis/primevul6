int nghttp2_session_on_goaway_received(nghttp2_session *session,
                                       nghttp2_frame *frame) {
  int rv;

  if (frame->hd.stream_id != 0) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "GOAWAY: stream_id != 0");
  }
  /* Spec says Endpoints MUST NOT increase the value they send in the
     last stream identifier. */
  if ((frame->goaway.last_stream_id > 0 &&
       !nghttp2_session_is_my_stream_id(session,
                                        frame->goaway.last_stream_id)) ||
      session->remote_last_stream_id < frame->goaway.last_stream_id) {
    return session_handle_invalid_connection(session, frame, NGHTTP2_ERR_PROTO,
                                             "GOAWAY: invalid last_stream_id");
  }

  session->goaway_flags |= NGHTTP2_GOAWAY_RECV;

  session->remote_last_stream_id = frame->goaway.last_stream_id;

  rv = session_call_on_frame_received(session, frame);

  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  return session_close_stream_on_goaway(session, frame->goaway.last_stream_id,
                                        0);
}