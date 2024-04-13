static int update_remote_initial_window_size_func(nghttp2_map_entry *entry,
                                                  void *ptr) {
  int rv;
  nghttp2_update_window_size_arg *arg;
  nghttp2_stream *stream;

  arg = (nghttp2_update_window_size_arg *)ptr;
  stream = (nghttp2_stream *)entry;

  rv = nghttp2_stream_update_remote_initial_window_size(
      stream, arg->new_window_size, arg->old_window_size);
  if (rv != 0) {
    return nghttp2_session_add_rst_stream(arg->session, stream->stream_id,
                                          NGHTTP2_FLOW_CONTROL_ERROR);
  }

  /* If window size gets positive, push deferred DATA frame to
     outbound queue. */
  if (stream->remote_window_size > 0 &&
      nghttp2_stream_check_deferred_by_flow_control(stream)) {

    rv = nghttp2_stream_resume_deferred_item(
        stream, NGHTTP2_STREAM_FLAG_DEFERRED_FLOW_CONTROL);

    if (nghttp2_is_fatal(rv)) {
      return rv;
    }
  }
  return 0;
}