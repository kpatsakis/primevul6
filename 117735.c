static int session_enforce_http_messaging(nghttp2_session *session) {
  return (session->opt_flags & NGHTTP2_OPTMASK_NO_HTTP_MESSAGING) == 0;
}