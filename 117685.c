session_append_inflight_settings(nghttp2_session *session,
                                 nghttp2_inflight_settings *settings) {
  nghttp2_inflight_settings **i;

  for (i = &session->inflight_settings_head; *i; i = &(*i)->next)
    ;

  *i = settings;
}