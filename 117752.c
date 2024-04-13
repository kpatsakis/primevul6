int nghttp2_session_destroy_stream(nghttp2_session *session,
                                   nghttp2_stream *stream) {
  nghttp2_mem *mem;
  int rv;

  DEBUGF("stream: destroy closed stream(%p)=%d\n", stream, stream->stream_id);

  mem = &session->mem;

  if (nghttp2_stream_in_dep_tree(stream)) {
    rv = nghttp2_stream_dep_remove(stream);
    if (rv != 0) {
      return rv;
    }
  }

  nghttp2_map_remove(&session->streams, stream->stream_id);
  nghttp2_stream_free(stream);
  nghttp2_mem_free(mem, stream);

  return 0;
}