int nghttp2_session_consume_connection(nghttp2_session *session, size_t size) {
  int rv;

  if (!(session->opt_flags & NGHTTP2_OPTMASK_NO_AUTO_WINDOW_UPDATE)) {
    return NGHTTP2_ERR_INVALID_STATE;
  }

  rv = session_update_connection_consumed_size(session, size);

  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  return 0;
}