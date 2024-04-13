session_update_local_initial_window_size(nghttp2_session *session,
                                         int32_t new_initial_window_size,
                                         int32_t old_initial_window_size) {
  nghttp2_update_window_size_arg arg;
  arg.session = session;
  arg.new_window_size = new_initial_window_size;
  arg.old_window_size = old_initial_window_size;
  return nghttp2_map_each(&session->streams,
                          update_local_initial_window_size_func, &arg);
}