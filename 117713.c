static ssize_t session_recv(nghttp2_session *session, uint8_t *buf,
                            size_t len) {
  ssize_t rv;
  rv = session->callbacks.recv_callback(session, buf, len, 0,
                                        session->user_data);
  if (rv > 0) {
    if ((size_t)rv > len) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  } else if (rv < 0 && rv != NGHTTP2_ERR_WOULDBLOCK && rv != NGHTTP2_ERR_EOF) {
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  return rv;
}