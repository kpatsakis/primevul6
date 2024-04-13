static int session_headers_add_pad(nghttp2_session *session,
                                   nghttp2_frame *frame) {
  int rv;
  ssize_t padded_payloadlen;
  nghttp2_active_outbound_item *aob;
  nghttp2_bufs *framebufs;
  size_t padlen;
  size_t max_payloadlen;

  aob = &session->aob;
  framebufs = &aob->framebufs;

  max_payloadlen = nghttp2_min(NGHTTP2_MAX_PAYLOADLEN,
                               frame->hd.length + NGHTTP2_MAX_PADLEN);

  padded_payloadlen =
      session_call_select_padding(session, frame, max_payloadlen);

  if (nghttp2_is_fatal((int)padded_payloadlen)) {
    return (int)padded_payloadlen;
  }

  padlen = (size_t)padded_payloadlen - frame->hd.length;

  DEBUGF("send: padding selected: payloadlen=%zd, padlen=%zu\n",
         padded_payloadlen, padlen);

  rv = nghttp2_frame_add_pad(framebufs, &frame->hd, padlen, 0);

  if (rv != 0) {
    return rv;
  }

  frame->headers.padlen = padlen;

  return 0;
}