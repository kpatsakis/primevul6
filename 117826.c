int nghttp2_session_add_rst_stream(nghttp2_session *session, int32_t stream_id,
                                   uint32_t error_code) {
  int rv;
  nghttp2_outbound_item *item;
  nghttp2_frame *frame;
  nghttp2_stream *stream;
  nghttp2_mem *mem;

  mem = &session->mem;
  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream && stream->state == NGHTTP2_STREAM_CLOSING) {
    return 0;
  }

  /* Cancel pending request HEADERS in ob_syn if this RST_STREAM
     refers to that stream. */
  if (!session->server && nghttp2_session_is_my_stream_id(session, stream_id) &&
      nghttp2_outbound_queue_top(&session->ob_syn)) {
    nghttp2_headers_aux_data *aux_data;
    nghttp2_frame *headers_frame;

    headers_frame = &nghttp2_outbound_queue_top(&session->ob_syn)->frame;
    assert(headers_frame->hd.type == NGHTTP2_HEADERS);

    if (headers_frame->hd.stream_id <= stream_id &&
        (uint32_t)stream_id < session->next_stream_id) {

      for (item = session->ob_syn.head; item; item = item->qnext) {
        aux_data = &item->aux_data.headers;

        if (item->frame.hd.stream_id < stream_id) {
          continue;
        }

        /* stream_id in ob_syn queue must be strictly increasing.  If
           we found larger ID, then we can break here. */
        if (item->frame.hd.stream_id > stream_id || aux_data->canceled) {
          break;
        }

        aux_data->error_code = error_code;
        aux_data->canceled = 1;

        return 0;
      }
    }
  }

  item = nghttp2_mem_malloc(mem, sizeof(nghttp2_outbound_item));
  if (item == NULL) {
    return NGHTTP2_ERR_NOMEM;
  }

  nghttp2_outbound_item_init(item);

  frame = &item->frame;

  nghttp2_frame_rst_stream_init(&frame->rst_stream, stream_id, error_code);
  rv = nghttp2_session_add_item(session, item);
  if (rv != 0) {
    nghttp2_frame_rst_stream_free(&frame->rst_stream);
    nghttp2_mem_free(mem, item);
    return rv;
  }
  return 0;
}