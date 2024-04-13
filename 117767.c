static int update_local_initial_window_size_func(nghttp2_map_entry *entry,
                                                 void *ptr) {
  int rv;
  nghttp2_update_window_size_arg *arg;
  nghttp2_stream *stream;
  arg = (nghttp2_update_window_size_arg *)ptr;
  stream = (nghttp2_stream *)entry;
  rv = nghttp2_stream_update_local_initial_window_size(
      stream, arg->new_window_size, arg->old_window_size);
  if (rv != 0) {
    return nghttp2_session_add_rst_stream(arg->session, stream->stream_id,
                                          NGHTTP2_FLOW_CONTROL_ERROR);
  }
  if (!(arg->session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_WINDOW_UPDATE) &&
      stream->window_update_queued == 0 &&
      nghttp2_should_send_window_update(stream->local_window_size,
                                        stream->recv_window_size)) {

    rv = nghttp2_session_add_window_update(arg->session, NGHTTP2_FLAG_NONE,
                                           stream->stream_id,
                                           stream->recv_window_size);
    if (rv != 0) {
      return rv;
    }

    stream->recv_window_size = 0;
  }
  return 0;
}