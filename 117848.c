static int session_call_error_callback(nghttp2_session *session,
                                       int lib_error_code, const char *fmt,
                                       ...) {
  size_t bufsize;
  va_list ap;
  char *buf;
  int rv;
  nghttp2_mem *mem;

  if (!session->callbacks.error_callback &&
      !session->callbacks.error_callback2) {
    return 0;
  }

  mem = &session->mem;

  va_start(ap, fmt);
  rv = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);

  if (rv < 0) {
    return NGHTTP2_ERR_NOMEM;
  }

  bufsize = (size_t)(rv + 1);

  buf = nghttp2_mem_malloc(mem, bufsize);
  if (buf == NULL) {
    return NGHTTP2_ERR_NOMEM;
  }

  va_start(ap, fmt);
  rv = vsnprintf(buf, bufsize, fmt, ap);
  va_end(ap);

  if (rv < 0) {
    nghttp2_mem_free(mem, buf);
    /* vsnprintf may return error because of various things we can
       imagine, but typically we don't want to drop session just for
       debug callback. */
    DEBUGF("error_callback: vsnprintf failed. The template was %s\n", fmt);
    return 0;
  }

  if (session->callbacks.error_callback2) {
    rv = session->callbacks.error_callback2(session, lib_error_code, buf,
                                            (size_t)rv, session->user_data);
  } else {
    rv = session->callbacks.error_callback(session, buf, (size_t)rv,
                                           session->user_data);
  }

  nghttp2_mem_free(mem, buf);

  if (rv != 0) {
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  return 0;
}