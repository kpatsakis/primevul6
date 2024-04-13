static ssize_t session_call_select_padding(nghttp2_session *session,
                                           const nghttp2_frame *frame,
                                           size_t max_payloadlen) {
  ssize_t rv;

  if (frame->hd.length >= max_payloadlen) {
    return (ssize_t)frame->hd.length;
  }

  if (session->callbacks.select_padding_callback) {
    size_t max_paddedlen;

    max_paddedlen =
        nghttp2_min(frame->hd.length + NGHTTP2_MAX_PADLEN, max_payloadlen);

    rv = session->callbacks.select_padding_callback(
        session, frame, max_paddedlen, session->user_data);
    if (rv < (ssize_t)frame->hd.length || rv > (ssize_t)max_paddedlen) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    return rv;
  }
  return (ssize_t)frame->hd.length;
}