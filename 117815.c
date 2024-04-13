static int session_call_send_data(nghttp2_session *session,
                                  nghttp2_outbound_item *item,
                                  nghttp2_bufs *framebufs) {
  int rv;
  nghttp2_buf *buf;
  size_t length;
  nghttp2_frame *frame;
  nghttp2_data_aux_data *aux_data;

  buf = &framebufs->cur->buf;
  frame = &item->frame;
  length = frame->hd.length - frame->data.padlen;
  aux_data = &item->aux_data.data;

  rv = session->callbacks.send_data_callback(session, frame, buf->pos, length,
                                             &aux_data->data_prd.source,
                                             session->user_data);

  switch (rv) {
  case 0:
  case NGHTTP2_ERR_WOULDBLOCK:
  case NGHTTP2_ERR_PAUSE:
  case NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE:
    return rv;
  default:
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }
}