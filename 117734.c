int nghttp2_session_set_stream_user_data(nghttp2_session *session,
                                         int32_t stream_id,
                                         void *stream_user_data) {
  nghttp2_stream *stream;
  nghttp2_frame *frame;
  nghttp2_outbound_item *item;

  stream = nghttp2_session_get_stream(session, stream_id);
  if (stream) {
    stream->stream_user_data = stream_user_data;
    return 0;
  }

  if (session->server || !nghttp2_session_is_my_stream_id(session, stream_id) ||
      !nghttp2_outbound_queue_top(&session->ob_syn)) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  frame = &nghttp2_outbound_queue_top(&session->ob_syn)->frame;
  assert(frame->hd.type == NGHTTP2_HEADERS);

  if (frame->hd.stream_id > stream_id ||
      (uint32_t)stream_id >= session->next_stream_id) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  for (item = session->ob_syn.head; item; item = item->qnext) {
    if (item->frame.hd.stream_id < stream_id) {
      continue;
    }

    if (item->frame.hd.stream_id > stream_id) {
      break;
    }

    item->aux_data.headers.stream_user_data = stream_user_data;
    return 0;
  }

  return NGHTTP2_ERR_INVALID_ARGUMENT;
}