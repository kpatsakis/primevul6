void nghttp2_session_set_user_data(nghttp2_session *session, void *user_data) {
  session->user_data = user_data;
}