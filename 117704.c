int nghttp2_session_recv(nghttp2_session *session) {
  uint8_t buf[NGHTTP2_INBOUND_BUFFER_LENGTH];
  while (1) {
    ssize_t readlen;
    readlen = session_recv(session, buf, sizeof(buf));
    if (readlen > 0) {
      ssize_t proclen = nghttp2_session_mem_recv(session, buf, (size_t)readlen);
      if (proclen < 0) {
        return (int)proclen;
      }
      assert(proclen == readlen);
    } else if (readlen == 0 || readlen == NGHTTP2_ERR_WOULDBLOCK) {
      return 0;
    } else if (readlen == NGHTTP2_ERR_EOF) {
      return NGHTTP2_ERR_EOF;
    } else if (readlen < 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }
}