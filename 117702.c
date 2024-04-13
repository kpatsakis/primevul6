static int session_after_frame_sent2(nghttp2_session *session) {
  int rv;
  nghttp2_active_outbound_item *aob = &session->aob;
  nghttp2_outbound_item *item = aob->item;
  nghttp2_bufs *framebufs = &aob->framebufs;
  nghttp2_frame *frame;
  nghttp2_mem *mem;
  nghttp2_stream *stream;
  nghttp2_data_aux_data *aux_data;

  mem = &session->mem;
  frame = &item->frame;

  if (frame->hd.type != NGHTTP2_DATA) {

    if (frame->hd.type == NGHTTP2_HEADERS ||
        frame->hd.type == NGHTTP2_PUSH_PROMISE) {

      if (nghttp2_bufs_next_present(framebufs)) {
        framebufs->cur = framebufs->cur->next;

        DEBUGF("send: next CONTINUATION frame, %zu bytes\n",
               nghttp2_buf_len(&framebufs->cur->buf));

        return 0;
      }
    }

    active_outbound_item_reset(&session->aob, mem);

    return 0;
  }

  /* DATA frame */

  aux_data = &item->aux_data.data;

  /* On EOF, we have already detached data.  Please note that
     application may issue nghttp2_submit_data() in
     on_frame_send_callback (call from session_after_frame_sent1),
     which attach data to stream.  We don't want to detach it. */
  if (aux_data->eof) {
    active_outbound_item_reset(aob, mem);

    return 0;
  }

  /* Reset no_copy here because next write may not use this. */
  aux_data->no_copy = 0;

  stream = nghttp2_session_get_stream(session, frame->hd.stream_id);

  /* If session is closed or RST_STREAM was queued, we won't send
     further data. */
  if (nghttp2_session_predicate_data_send(session, stream) != 0) {
    if (stream) {
      rv = nghttp2_stream_detach_item(stream);

      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
    }

    active_outbound_item_reset(aob, mem);

    return 0;
  }

  aob->item = NULL;
  active_outbound_item_reset(&session->aob, mem);

  return 0;
}