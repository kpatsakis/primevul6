static int inflate_header_block(nghttp2_session *session, nghttp2_frame *frame,
                                size_t *readlen_ptr, uint8_t *in, size_t inlen,
                                int final, int call_header_cb) {
  ssize_t proclen;
  int rv;
  int inflate_flags;
  nghttp2_hd_nv nv;
  nghttp2_stream *stream;
  nghttp2_stream *subject_stream;
  int trailer = 0;

  *readlen_ptr = 0;
  stream = nghttp2_session_get_stream(session, frame->hd.stream_id);

  if (frame->hd.type == NGHTTP2_PUSH_PROMISE) {
    subject_stream = nghttp2_session_get_stream(
        session, frame->push_promise.promised_stream_id);
  } else {
    subject_stream = stream;
    trailer = session_trailer_headers(session, stream, frame);
  }

  DEBUGF("recv: decoding header block %zu bytes\n", inlen);
  for (;;) {
    inflate_flags = 0;
    proclen = nghttp2_hd_inflate_hd_nv(&session->hd_inflater, &nv,
                                       &inflate_flags, in, inlen, final);
    if (nghttp2_is_fatal((int)proclen)) {
      return (int)proclen;
    }
    if (proclen < 0) {
      if (session->iframe.state == NGHTTP2_IB_READ_HEADER_BLOCK) {
        if (subject_stream && subject_stream->state != NGHTTP2_STREAM_CLOSING) {
          /* Adding RST_STREAM here is very important. It prevents
             from invoking subsequent callbacks for the same stream
             ID. */
          rv = nghttp2_session_add_rst_stream(
              session, subject_stream->stream_id, NGHTTP2_COMPRESSION_ERROR);

          if (nghttp2_is_fatal(rv)) {
            return rv;
          }
        }
      }
      rv =
          nghttp2_session_terminate_session(session, NGHTTP2_COMPRESSION_ERROR);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      return NGHTTP2_ERR_HEADER_COMP;
    }
    in += proclen;
    inlen -= (size_t)proclen;
    *readlen_ptr += (size_t)proclen;

    DEBUGF("recv: proclen=%zd\n", proclen);

    if (call_header_cb && (inflate_flags & NGHTTP2_HD_INFLATE_EMIT)) {
      rv = 0;
      if (subject_stream) {
        if (session_enforce_http_messaging(session)) {
          rv = nghttp2_http_on_header(session, subject_stream, frame, &nv,
                                      trailer);

          if (rv == NGHTTP2_ERR_IGN_HTTP_HEADER) {
            /* Don't overwrite rv here */
            int rv2;

            rv2 = session_call_on_invalid_header(session, frame, &nv);
            if (rv2 == NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE) {
              rv = NGHTTP2_ERR_HTTP_HEADER;
            } else {
              if (rv2 != 0) {
                return rv2;
              }

              /* header is ignored */
              DEBUGF("recv: HTTP ignored: type=%u, id=%d, header %.*s: %.*s\n",
                     frame->hd.type, frame->hd.stream_id, (int)nv.name->len,
                     nv.name->base, (int)nv.value->len, nv.value->base);

              rv2 = session_call_error_callback(
                  session, NGHTTP2_ERR_HTTP_HEADER,
                  "Ignoring received invalid HTTP header field: frame type: "
                  "%u, stream: %d, name: [%.*s], value: [%.*s]",
                  frame->hd.type, frame->hd.stream_id, (int)nv.name->len,
                  nv.name->base, (int)nv.value->len, nv.value->base);

              if (nghttp2_is_fatal(rv2)) {
                return rv2;
              }
            }
          }

          if (rv == NGHTTP2_ERR_HTTP_HEADER) {
            DEBUGF("recv: HTTP error: type=%u, id=%d, header %.*s: %.*s\n",
                   frame->hd.type, frame->hd.stream_id, (int)nv.name->len,
                   nv.name->base, (int)nv.value->len, nv.value->base);

            rv = session_call_error_callback(
                session, NGHTTP2_ERR_HTTP_HEADER,
                "Invalid HTTP header field was received: frame type: "
                "%u, stream: %d, name: [%.*s], value: [%.*s]",
                frame->hd.type, frame->hd.stream_id, (int)nv.name->len,
                nv.name->base, (int)nv.value->len, nv.value->base);

            if (nghttp2_is_fatal(rv)) {
              return rv;
            }

            rv = session_handle_invalid_stream2(session,
                                                subject_stream->stream_id,
                                                frame, NGHTTP2_ERR_HTTP_HEADER);
            if (nghttp2_is_fatal(rv)) {
              return rv;
            }
            return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
          }
        }
        if (rv == 0) {
          rv = session_call_on_header(session, frame, &nv);
          /* This handles NGHTTP2_ERR_PAUSE and
             NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE as well */
          if (rv != 0) {
            return rv;
          }
        }
      }
    }
    if (inflate_flags & NGHTTP2_HD_INFLATE_FINAL) {
      nghttp2_hd_inflate_end_headers(&session->hd_inflater);
      break;
    }
    if ((inflate_flags & NGHTTP2_HD_INFLATE_EMIT) == 0 && inlen == 0) {
      break;
    }
  }
  return 0;
}