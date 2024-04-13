static int session_end_stream_headers_received(nghttp2_session *session,
                                               nghttp2_frame *frame,
                                               nghttp2_stream *stream) {
  int rv;
  if ((frame->hd.flags & NGHTTP2_FLAG_END_STREAM) == 0) {
    return 0;
  }

  nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_RD);
  rv = nghttp2_session_close_stream_if_shut_rdwr(session, stream);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  return 0;
}