static int session_trailer_headers(nghttp2_session *session,
                                   nghttp2_stream *stream,
                                   nghttp2_frame *frame) {
  if (!stream || frame->hd.type != NGHTTP2_HEADERS) {
    return 0;
  }
  if (session->server) {
    return frame->headers.cat == NGHTTP2_HCAT_HEADERS;
  }

  return frame->headers.cat == NGHTTP2_HCAT_HEADERS &&
         (stream->http_flags & NGHTTP2_HTTP_FLAG_EXPECT_FINAL_RESPONSE) == 0;
}