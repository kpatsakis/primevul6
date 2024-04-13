int nghttp2_session_upgrade2(nghttp2_session *session,
                             const uint8_t *settings_payload,
                             size_t settings_payloadlen, int head_request,
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

  if (head_request) {
    stream->http_flags |= NGHTTP2_HTTP_FLAG_METH_HEAD;
  }

  return 0;
}