int nghttp2_session_send(nghttp2_session *session) {
  const uint8_t *data = NULL;
  ssize_t datalen;
  ssize_t sentlen;
  nghttp2_bufs *framebufs;

  framebufs = &session->aob.framebufs;

  for (;;) {
    datalen = nghttp2_session_mem_send_internal(session, &data, 0);
    if (datalen <= 0) {
      return (int)datalen;
    }
    sentlen = session->callbacks.send_callback(session, data, (size_t)datalen,
                                               0, session->user_data);
    if (sentlen < 0) {
      if (sentlen == NGHTTP2_ERR_WOULDBLOCK) {
        /* Transmission canceled. Rewind the offset */
        framebufs->cur->buf.pos -= datalen;

        return 0;
      }
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* Rewind the offset to the amount of unsent bytes */
    framebufs->cur->buf.pos -= datalen - sentlen;
  }
}