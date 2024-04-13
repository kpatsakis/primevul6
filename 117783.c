static int session_prep_frame(nghttp2_session *session,
                              nghttp2_outbound_item *item) {
  int rv;
  nghttp2_frame *frame;
  nghttp2_mem *mem;

  mem = &session->mem;
  frame = &item->frame;

  switch (frame->hd.type) {
  case NGHTTP2_DATA: {
    size_t next_readmax;
    nghttp2_stream *stream;

    stream = nghttp2_session_get_stream(session, frame->hd.stream_id);

    if (stream) {
      assert(stream->item == item);
    }

    rv = nghttp2_session_predicate_data_send(session, stream);
    if (rv != 0) {
      // If stream was already closed, nghttp2_session_get_stream()
      // returns NULL, but item is still attached to the stream.
      // Search stream including closed again.
      stream = nghttp2_session_get_stream_raw(session, frame->hd.stream_id);
      if (stream) {
        int rv2;

        rv2 = nghttp2_stream_detach_item(stream);

        if (nghttp2_is_fatal(rv2)) {
          return rv2;
        }
      }

      return rv;
    }
    /* Assuming stream is not NULL */
    assert(stream);
    next_readmax = nghttp2_session_next_data_read(session, stream);

    if (next_readmax == 0) {

      /* This must be true since we only pop DATA frame item from
         queue when session->remote_window_size > 0 */
      assert(session->remote_window_size > 0);

      rv = nghttp2_stream_defer_item(stream,
                                     NGHTTP2_STREAM_FLAG_DEFERRED_FLOW_CONTROL);

      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      session->aob.item = NULL;
      active_outbound_item_reset(&session->aob, mem);
      return NGHTTP2_ERR_DEFERRED;
    }

    rv = nghttp2_session_pack_data(session, &session->aob.framebufs,
                                   next_readmax, frame, &item->aux_data.data,
                                   stream);
    if (rv == NGHTTP2_ERR_PAUSE) {
      return rv;
    }
    if (rv == NGHTTP2_ERR_DEFERRED) {
      rv = nghttp2_stream_defer_item(stream, NGHTTP2_STREAM_FLAG_DEFERRED_USER);

      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      session->aob.item = NULL;
      active_outbound_item_reset(&session->aob, mem);
      return NGHTTP2_ERR_DEFERRED;
    }
    if (rv == NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE) {
      rv = nghttp2_stream_detach_item(stream);

      if (nghttp2_is_fatal(rv)) {
        return rv;
      }

      rv = nghttp2_session_add_rst_stream(session, frame->hd.stream_id,
                                          NGHTTP2_INTERNAL_ERROR);
      if (nghttp2_is_fatal(rv)) {
        return rv;
      }
      return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
    }
    if (rv != 0) {
      int rv2;

      rv2 = nghttp2_stream_detach_item(stream);

      if (nghttp2_is_fatal(rv2)) {
        return rv2;
      }

      return rv;
    }
    return 0;
  }
  case NGHTTP2_HEADERS: {
    nghttp2_headers_aux_data *aux_data;
    size_t estimated_payloadlen;

    aux_data = &item->aux_data.headers;

    if (frame->headers.cat == NGHTTP2_HCAT_REQUEST) {
      /* initial HEADERS, which opens stream */
      nghttp2_stream *stream;

      stream = nghttp2_session_open_stream(
          session, frame->hd.stream_id, NGHTTP2_STREAM_FLAG_NONE,
          &frame->headers.pri_spec, NGHTTP2_STREAM_INITIAL,
          aux_data->stream_user_data);

      if (stream == NULL) {
        return NGHTTP2_ERR_NOMEM;
      }

      /* We don't call nghttp2_session_adjust_closed_stream() here,
         since we don't keep closed stream in client side */

      rv = session_predicate_request_headers_send(session, item);
      if (rv != 0) {
        return rv;
      }

      if (session_enforce_http_messaging(session)) {
        nghttp2_http_record_request_method(stream, frame);
      }
    } else {
      nghttp2_stream *stream;

      stream = nghttp2_session_get_stream(session, frame->hd.stream_id);

      if (stream && stream->state == NGHTTP2_STREAM_RESERVED) {
        rv = session_predicate_push_response_headers_send(session, stream);
        if (rv == 0) {
          frame->headers.cat = NGHTTP2_HCAT_PUSH_RESPONSE;

          if (aux_data->stream_user_data) {
            stream->stream_user_data = aux_data->stream_user_data;
          }
        }
      } else if (session_predicate_response_headers_send(session, stream) ==
                 0) {
        frame->headers.cat = NGHTTP2_HCAT_RESPONSE;
        rv = 0;
      } else {
        frame->headers.cat = NGHTTP2_HCAT_HEADERS;

        rv = session_predicate_headers_send(session, stream);
      }

      if (rv != 0) {
        return rv;
      }
    }

    estimated_payloadlen = session_estimate_headers_payload(
        session, frame->headers.nva, frame->headers.nvlen,
        NGHTTP2_PRIORITY_SPECLEN);

    if (estimated_payloadlen > session->max_send_header_block_length) {
      return NGHTTP2_ERR_FRAME_SIZE_ERROR;
    }

    rv = nghttp2_frame_pack_headers(&session->aob.framebufs, &frame->headers,
                                    &session->hd_deflater);

    if (rv != 0) {
      return rv;
    }

    DEBUGF("send: before padding, HEADERS serialized in %zd bytes\n",
           nghttp2_bufs_len(&session->aob.framebufs));

    rv = session_headers_add_pad(session, frame);

    if (rv != 0) {
      return rv;
    }

    DEBUGF("send: HEADERS finally serialized in %zd bytes\n",
           nghttp2_bufs_len(&session->aob.framebufs));

    if (frame->headers.cat == NGHTTP2_HCAT_REQUEST) {
      assert(session->last_sent_stream_id < frame->hd.stream_id);
      session->last_sent_stream_id = frame->hd.stream_id;
    }

    return 0;
  }
  case NGHTTP2_PRIORITY: {
    if (session_is_closing(session)) {
      return NGHTTP2_ERR_SESSION_CLOSING;
    }
    /* PRIORITY frame can be sent at any time and to any stream
       ID. */
    nghttp2_frame_pack_priority(&session->aob.framebufs, &frame->priority);

    /* Peer can send PRIORITY frame against idle stream to create
       "anchor" in dependency tree.  Only client can do this in
       nghttp2.  In nghttp2, only server retains non-active (closed
       or idle) streams in memory, so we don't open stream here. */
    return 0;
  }
  case NGHTTP2_RST_STREAM:
    if (session_is_closing(session)) {
      return NGHTTP2_ERR_SESSION_CLOSING;
    }
    nghttp2_frame_pack_rst_stream(&session->aob.framebufs, &frame->rst_stream);
    return 0;
  case NGHTTP2_SETTINGS: {
    if (frame->hd.flags & NGHTTP2_FLAG_ACK) {
      assert(session->obq_flood_counter_ > 0);
      --session->obq_flood_counter_;
      /* When session is about to close, don't send SETTINGS ACK.
         We are required to send SETTINGS without ACK though; for
         example, we have to send SETTINGS as a part of connection
         preface. */
      if (session_is_closing(session)) {
        return NGHTTP2_ERR_SESSION_CLOSING;
      }
    }

    rv = nghttp2_frame_pack_settings(&session->aob.framebufs, &frame->settings);
    if (rv != 0) {
      return rv;
    }
    return 0;
  }
  case NGHTTP2_PUSH_PROMISE: {
    nghttp2_stream *stream;
    size_t estimated_payloadlen;

    /* stream could be NULL if associated stream was already
       closed. */
    stream = nghttp2_session_get_stream(session, frame->hd.stream_id);

    /* predicate should fail if stream is NULL. */
    rv = session_predicate_push_promise_send(session, stream);
    if (rv != 0) {
      return rv;
    }

    assert(stream);

    estimated_payloadlen = session_estimate_headers_payload(
        session, frame->push_promise.nva, frame->push_promise.nvlen, 0);

    if (estimated_payloadlen > session->max_send_header_block_length) {
      return NGHTTP2_ERR_FRAME_SIZE_ERROR;
    }

    rv = nghttp2_frame_pack_push_promise(
        &session->aob.framebufs, &frame->push_promise, &session->hd_deflater);
    if (rv != 0) {
      return rv;
    }
    rv = session_headers_add_pad(session, frame);
    if (rv != 0) {
      return rv;
    }

    assert(session->last_sent_stream_id + 2 <=
           frame->push_promise.promised_stream_id);
    session->last_sent_stream_id = frame->push_promise.promised_stream_id;

    return 0;
  }
  case NGHTTP2_PING:
    if (frame->hd.flags & NGHTTP2_FLAG_ACK) {
      assert(session->obq_flood_counter_ > 0);
      --session->obq_flood_counter_;
    }
    /* PING frame is allowed to be sent unless termination GOAWAY is
       sent */
    if (session->goaway_flags & NGHTTP2_GOAWAY_TERM_ON_SEND) {
      return NGHTTP2_ERR_SESSION_CLOSING;
    }
    nghttp2_frame_pack_ping(&session->aob.framebufs, &frame->ping);
    return 0;
  case NGHTTP2_GOAWAY:
    rv = nghttp2_frame_pack_goaway(&session->aob.framebufs, &frame->goaway);
    if (rv != 0) {
      return rv;
    }
    session->local_last_stream_id = frame->goaway.last_stream_id;

    return 0;
  case NGHTTP2_WINDOW_UPDATE:
    rv = session_predicate_window_update_send(session, frame->hd.stream_id);
    if (rv != 0) {
      return rv;
    }
    nghttp2_frame_pack_window_update(&session->aob.framebufs,
                                     &frame->window_update);
    return 0;
  case NGHTTP2_CONTINUATION:
    /* We never handle CONTINUATION here. */
    assert(0);
    return 0;
  default: {
    nghttp2_ext_aux_data *aux_data;

    /* extension frame */

    aux_data = &item->aux_data.ext;

    if (aux_data->builtin == 0) {
      if (session_is_closing(session)) {
        return NGHTTP2_ERR_SESSION_CLOSING;
      }

      return session_pack_extension(session, &session->aob.framebufs, frame);
    }

    switch (frame->hd.type) {
    case NGHTTP2_ALTSVC:
      rv = session_predicate_altsvc_send(session, frame->hd.stream_id);
      if (rv != 0) {
        return rv;
      }

      nghttp2_frame_pack_altsvc(&session->aob.framebufs, &frame->ext);

      return 0;
    case NGHTTP2_ORIGIN:
      rv = session_predicate_origin_send(session);
      if (rv != 0) {
        return rv;
      }

      rv = nghttp2_frame_pack_origin(&session->aob.framebufs, &frame->ext);
      if (rv != 0) {
        return rv;
      }

      return 0;
    default:
      /* Unreachable here */
      assert(0);
      return 0;
    }
  }
  }
}