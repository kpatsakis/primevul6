ssize_t nghttp2_session_mem_send(nghttp2_session *session,
                                 const uint8_t **data_ptr) {
  int rv;
  ssize_t len;

  *data_ptr = NULL;

  len = nghttp2_session_mem_send_internal(session, data_ptr, 1);
  if (len <= 0) {
    return len;
  }

  if (session->aob.item) {
    /* We have to call session_after_frame_sent1 here to handle stream
       closure upon transmission of frames.  Otherwise, END_STREAM may
       be reached to client before we call nghttp2_session_mem_send
       again and we may get exceeding number of incoming streams. */
    rv = session_after_frame_sent1(session);
    if (rv < 0) {
      assert(nghttp2_is_fatal(rv));
      return (ssize_t)rv;
    }
  }

  return len;
}