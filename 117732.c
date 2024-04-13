int nghttp2_session_close_stream_if_shut_rdwr(nghttp2_session *session,
                                              nghttp2_stream *stream) {
  if ((stream->shut_flags & NGHTTP2_SHUT_RDWR) == NGHTTP2_SHUT_RDWR) {
    return nghttp2_session_close_stream(session, stream->stream_id,
                                        NGHTTP2_NO_ERROR);
  }
  return 0;
}