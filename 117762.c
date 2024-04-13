static int session_call_on_header(nghttp2_session *session,
                                  const nghttp2_frame *frame,
                                  const nghttp2_hd_nv *nv) {
  int rv = 0;
  if (session->callbacks.on_header_callback2) {
    rv = session->callbacks.on_header_callback2(
        session, frame, nv->name, nv->value, nv->flags, session->user_data);
  } else if (session->callbacks.on_header_callback) {
    rv = session->callbacks.on_header_callback(
        session, frame, nv->name->base, nv->name->len, nv->value->base,
        nv->value->len, nv->flags, session->user_data);
  }

  if (rv == NGHTTP2_ERR_PAUSE || rv == NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE) {
    return rv;
  }
  if (rv != 0) {
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  return 0;
}