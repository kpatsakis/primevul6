int nghttp2_session_close_stream(nghttp2_session *session, int32_t stream_id,
                                 uint32_t error_code) {
  int rv;
  nghttp2_stream *stream;
  nghttp2_mem *mem;
  int is_my_stream_id;

  mem = &session->mem;
  stream = nghttp2_session_get_stream(session, stream_id);

  if (!stream) {
    return NGHTTP2_ERR_INVALID_ARGUMENT;
  }

  DEBUGF("stream: stream(%p)=%d close\n", stream, stream->stream_id);

  if (stream->item) {
    nghttp2_outbound_item *item;

    item = stream->item;

    rv = nghttp2_stream_detach_item(stream);

    if (rv != 0) {
      return rv;
    }

    /* If item is queued, it will be deleted when it is popped
       (nghttp2_session_prep_frame() will fail).  If session->aob.item
       points to this item, let active_outbound_item_reset()
       free the item. */
    if (!item->queued && item != session->aob.item) {
      nghttp2_outbound_item_free(item, mem);
      nghttp2_mem_free(mem, item);
    }
  }

  /* We call on_stream_close_callback even if stream->state is
     NGHTTP2_STREAM_INITIAL. This will happen while sending request
     HEADERS, a local endpoint receives RST_STREAM for that stream. It
     may be PROTOCOL_ERROR, but without notifying stream closure will
     hang the stream in a local endpoint.
  */

  if (session->callbacks.on_stream_close_callback) {
    if (session->callbacks.on_stream_close_callback(
            session, stream_id, error_code, session->user_data) != 0) {

      return NGHTTP2_ERR_CALLBACK_FAILURE;
    }
  }

  is_my_stream_id = nghttp2_session_is_my_stream_id(session, stream_id);

  /* pushed streams which is not opened yet is not counted toward max
     concurrent limits */
  if ((stream->flags & NGHTTP2_STREAM_FLAG_PUSH)) {
    if (!is_my_stream_id) {
      --session->num_incoming_reserved_streams;
    }
  } else {
    if (is_my_stream_id) {
      --session->num_outgoing_streams;
    } else {
      --session->num_incoming_streams;
    }
  }

  /* Closes both directions just in case they are not closed yet */
  stream->flags |= NGHTTP2_STREAM_FLAG_CLOSED;

  if ((session->opt_flags & NGHTTP2_OPTMASK_NO_CLOSED_STREAMS) == 0 &&
      session->server && !is_my_stream_id &&
      nghttp2_stream_in_dep_tree(stream)) {
    /* On server side, retain stream at most MAX_CONCURRENT_STREAMS
       combined with the current active incoming streams to make
       dependency tree work better. */
    nghttp2_session_keep_closed_stream(session, stream);
  } else {
    rv = nghttp2_session_destroy_stream(session, stream);
    if (rv != 0) {
      return rv;
    }
  }

  return 0;
}