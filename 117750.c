static int session_pack_extension(nghttp2_session *session, nghttp2_bufs *bufs,
                                  nghttp2_frame *frame) {
  ssize_t rv;
  nghttp2_buf *buf;
  size_t buflen;
  size_t framelen;

  assert(session->callbacks.pack_extension_callback);

  buf = &bufs->head->buf;
  buflen = nghttp2_min(nghttp2_buf_avail(buf), NGHTTP2_MAX_PAYLOADLEN);

  rv = session->callbacks.pack_extension_callback(session, buf->last, buflen,
                                                  frame, session->user_data);
  if (rv == NGHTTP2_ERR_CANCEL) {
    return (int)rv;
  }

  if (rv < 0 || (size_t)rv > buflen) {
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  framelen = (size_t)rv;

  frame->hd.length = framelen;

  assert(buf->pos == buf->last);
  buf->last += framelen;
  buf->pos -= NGHTTP2_FRAME_HDLEN;

  nghttp2_frame_pack_frame_hd(buf->pos, &frame->hd);

  return 0;
}