static int state_reserved_local(nghttp2_session *session,
                                nghttp2_stream *stream) {
  return stream->state == NGHTTP2_STREAM_RESERVED &&
         nghttp2_session_is_my_stream_id(session, stream->stream_id);
}