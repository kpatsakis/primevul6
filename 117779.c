int nghttp2_session_on_altsvc_received(nghttp2_session *session,
                                       nghttp2_frame *frame) {
  nghttp2_ext_altsvc *altsvc;
  nghttp2_stream *stream;

  altsvc = frame->ext.payload;

  /* session->server case has been excluded */

  if (frame->hd.stream_id == 0) {
    if (altsvc->origin_len == 0) {
      return session_call_on_invalid_frame_recv_callback(session, frame,
                                                         NGHTTP2_ERR_PROTO);
    }
  } else {
    if (altsvc->origin_len > 0) {
      return session_call_on_invalid_frame_recv_callback(session, frame,
                                                         NGHTTP2_ERR_PROTO);
    }

    stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
    if (!stream) {
      return 0;
    }

    if (stream->state == NGHTTP2_STREAM_CLOSING) {
      return 0;
    }
  }

  if (altsvc->field_value_len == 0) {
    return session_call_on_invalid_frame_recv_callback(session, frame,
                                                       NGHTTP2_ERR_PROTO);
  }

  return session_call_on_frame_received(session, frame);
}