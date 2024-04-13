int nghttp2_session_upgrade(nghttp2_session *session,
                            const uint8_t *settings_payload,
                            size_t settings_payloadlen,
                            void *stream_user_data) {
  int rv;
  nghttp2_stream *stream;

  rv = nghttp2_session_upgrade_internal(session, settings_payload,
                                        settings_payloadlen, stream_user_data);
  if (rv != 0) {
    return rv;
  }

  stream = nghttp2_session_get_stream(session, 1);
  assert(stream);

  /* We have no information about request header fields when Upgrade
     was happened.  So we don't know the request method here.  If
     request method is HEAD, we have a trouble because we may have
     nonzero content-length header field in response headers, and we
     will going to check it against the actual DATA frames, but we may
     get mismatch because HEAD response body must be empty.  Because
     of this reason, nghttp2_session_upgrade() was deprecated in favor
     of nghttp2_session_upgrade2(), which has |head_request| parameter
     to indicate that request method is HEAD or not. */
  stream->http_flags |= NGHTTP2_HTTP_FLAG_METH_UPGRADE_WORKAROUND;
  return 0;
}