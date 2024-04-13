int nghttp2_session_pack_data(nghttp2_session *session, nghttp2_bufs *bufs,
                              size_t datamax, nghttp2_frame *frame,
                              nghttp2_data_aux_data *aux_data,
                              nghttp2_stream *stream) {
  int rv;
  uint32_t data_flags;
  ssize_t payloadlen;
  ssize_t padded_payloadlen;
  nghttp2_buf *buf;
  size_t max_payloadlen;

  assert(bufs->head == bufs->cur);

  buf = &bufs->cur->buf;

  if (session->callbacks.read_length_callback) {

    payloadlen = session->callbacks.read_length_callback(
        session, frame->hd.type, stream->stream_id, session->remote_window_size,
        stream->remote_window_size, session->remote_settings.max_frame_size,
        session->user_data);

    DEBUGF("send: read_length_callback=%zd\n", payloadlen);

    payloadlen = nghttp2_session_enforce_flow_control_limits(session, stream,
                                                             payloadlen);

    DEBUGF("send: read_length_callback after flow control=%zd\n", payloadlen);

    if (payloadlen <= 0) {
      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    if ((size_t)payloadlen > nghttp2_buf_avail(buf)) {
      /* Resize the current buffer(s).  The reason why we do +1 for
         buffer size is for possible padding field. */
      rv = nghttp2_bufs_realloc(&session->aob.framebufs,
                                (size_t)(NGHTTP2_FRAME_HDLEN + 1 + payloadlen));

      if (rv != 0) {
        DEBUGF("send: realloc buffer failed rv=%d", rv);
        /* If reallocation failed, old buffers are still in tact.  So
           use safe limit. */
        payloadlen = (ssize_t)datamax;

        DEBUGF("send: use safe limit payloadlen=%zd", payloadlen);
      } else {
        assert(&session->aob.framebufs == bufs);

        buf = &bufs->cur->buf;
      }
    }
    datamax = (size_t)payloadlen;
  }

  /* Current max DATA length is less then buffer chunk size */
  assert(nghttp2_buf_avail(buf) >= datamax);

  data_flags = NGHTTP2_DATA_FLAG_NONE;
  payloadlen = aux_data->data_prd.read_callback(
      session, frame->hd.stream_id, buf->pos, datamax, &data_flags,
      &aux_data->data_prd.source, session->user_data);

  if (payloadlen == NGHTTP2_ERR_DEFERRED ||
      payloadlen == NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE ||
      payloadlen == NGHTTP2_ERR_PAUSE) {
    DEBUGF("send: DATA postponed due to %s\n",
           nghttp2_strerror((int)payloadlen));

    return (int)payloadlen;
  }

  if (payloadlen < 0 || datamax < (size_t)payloadlen) {
    /* This is the error code when callback is failed. */
    return NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  buf->last = buf->pos + payloadlen;
  buf->pos -= NGHTTP2_FRAME_HDLEN;

  /* Clear flags, because this may contain previous flags of previous
     DATA */
  frame->hd.flags = NGHTTP2_FLAG_NONE;

  if (data_flags & NGHTTP2_DATA_FLAG_EOF) {
    aux_data->eof = 1;
    /* If NGHTTP2_DATA_FLAG_NO_END_STREAM is set, don't set
       NGHTTP2_FLAG_END_STREAM */
    if ((aux_data->flags & NGHTTP2_FLAG_END_STREAM) &&
        (data_flags & NGHTTP2_DATA_FLAG_NO_END_STREAM) == 0) {
      frame->hd.flags |= NGHTTP2_FLAG_END_STREAM;
    }
  }

  if (data_flags & NGHTTP2_DATA_FLAG_NO_COPY) {
    if (session->callbacks.send_data_callback == NULL) {
      DEBUGF("NGHTTP2_DATA_FLAG_NO_COPY requires send_data_callback set\n");

      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    aux_data->no_copy = 1;
  }

  frame->hd.length = (size_t)payloadlen;
  frame->data.padlen = 0;

  max_payloadlen = nghttp2_min(datamax, frame->hd.length + NGHTTP2_MAX_PADLEN);

  padded_payloadlen =
      session_call_select_padding(session, frame, max_payloadlen);

  if (nghttp2_is_fatal((int)padded_payloadlen)) {
    return (int)padded_payloadlen;
  }

  frame->data.padlen = (size_t)(padded_payloadlen - payloadlen);

  nghttp2_frame_pack_frame_hd(buf->pos, &frame->hd);

  rv = nghttp2_frame_add_pad(bufs, &frame->hd, frame->data.padlen,
                             aux_data->no_copy);
  if (rv != 0) {
    return rv;
  }

  reschedule_stream(stream);

  if (frame->hd.length == 0 && (data_flags & NGHTTP2_DATA_FLAG_EOF) &&
      (data_flags & NGHTTP2_DATA_FLAG_NO_END_STREAM)) {
    /* DATA payload length is 0, and DATA frame does not bear
       END_STREAM.  In this case, there is no point to send 0 length
       DATA frame. */
    return NGHTTP2_ERR_CANCEL;
  }

  return 0;
}