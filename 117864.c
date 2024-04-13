static size_t session_get_num_active_streams(nghttp2_session *session) {
  return nghttp2_map_size(&session->streams) - session->num_closed_streams -
         session->num_idle_streams;
}