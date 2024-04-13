static void session_inbound_frame_reset(nghttp2_session *session) {
  nghttp2_inbound_frame *iframe = &session->iframe;
  nghttp2_mem *mem = &session->mem;
  /* A bit risky code, since if this function is called from
     nghttp2_session_new(), we rely on the fact that
     iframe->frame.hd.type is 0, so that no free is performed. */
  switch (iframe->frame.hd.type) {
  case NGHTTP2_DATA:
    break;
  case NGHTTP2_HEADERS:
    nghttp2_frame_headers_free(&iframe->frame.headers, mem);
    break;
  case NGHTTP2_PRIORITY:
    nghttp2_frame_priority_free(&iframe->frame.priority);
    break;
  case NGHTTP2_RST_STREAM:
    nghttp2_frame_rst_stream_free(&iframe->frame.rst_stream);
    break;
  case NGHTTP2_SETTINGS:
    nghttp2_frame_settings_free(&iframe->frame.settings, mem);

    nghttp2_mem_free(mem, iframe->iv);

    iframe->iv = NULL;
    iframe->niv = 0;
    iframe->max_niv = 0;

    break;
  case NGHTTP2_PUSH_PROMISE:
    nghttp2_frame_push_promise_free(&iframe->frame.push_promise, mem);
    break;
  case NGHTTP2_PING:
    nghttp2_frame_ping_free(&iframe->frame.ping);
    break;
  case NGHTTP2_GOAWAY:
    nghttp2_frame_goaway_free(&iframe->frame.goaway, mem);
    break;
  case NGHTTP2_WINDOW_UPDATE:
    nghttp2_frame_window_update_free(&iframe->frame.window_update);
    break;
  default:
    /* extension frame */
    if (check_ext_type_set(session->user_recv_ext_types,
                           iframe->frame.hd.type)) {
      nghttp2_frame_extension_free(&iframe->frame.ext);
    } else {
      switch (iframe->frame.hd.type) {
      case NGHTTP2_ALTSVC:
        if ((session->builtin_recv_ext_types & NGHTTP2_TYPEMASK_ALTSVC) == 0) {
          break;
        }
        nghttp2_frame_altsvc_free(&iframe->frame.ext, mem);
        break;
      case NGHTTP2_ORIGIN:
        if ((session->builtin_recv_ext_types & NGHTTP2_TYPEMASK_ORIGIN) == 0) {
          break;
        }
        nghttp2_frame_origin_free(&iframe->frame.ext, mem);
        break;
      }
    }

    break;
  }

  memset(&iframe->frame, 0, sizeof(nghttp2_frame));
  memset(&iframe->ext_frame_payload, 0, sizeof(nghttp2_ext_frame_payload));

  iframe->state = NGHTTP2_IB_READ_HEAD;

  nghttp2_buf_wrap_init(&iframe->sbuf, iframe->raw_sbuf,
                        sizeof(iframe->raw_sbuf));
  iframe->sbuf.mark += NGHTTP2_FRAME_HDLEN;

  nghttp2_buf_free(&iframe->lbuf, mem);
  nghttp2_buf_wrap_init(&iframe->lbuf, NULL, 0);

  iframe->raw_lbuf = NULL;

  iframe->payloadleft = 0;
  iframe->padlen = 0;
}