static ssize_t nghttp2_session_mem_send_internal(nghttp2_session *session,
                                                 const uint8_t **data_ptr,
                                                 int fast_cb) {
  int rv;
  nghttp2_active_outbound_item *aob;
  nghttp2_bufs *framebufs;
  nghttp2_mem *mem;

  mem = &session->mem;
  aob = &session->aob;
  framebufs = &aob->framebufs;

  /* We may have idle streams more than we expect (e.g.,
     nghttp2_session_change_stream_priority() or
     nghttp2_session_create_idle_stream()).  Adjust them here. */
  rv = nghttp2_session_adjust_idle_stream(session);
  if (nghttp2_is_fatal(rv)) {
    return rv;
  }

  for (;;) {
    switch (aob->state) {
    case NGHTTP2_OB_POP_ITEM: {
      nghttp2_outbound_item *item;

      item = nghttp2_session_pop_next_ob_item(session);
      if (item == NULL) {
        return 0;
      }

      rv = session_prep_frame(session, item);
      if (rv == NGHTTP2_ERR_PAUSE) {
        return 0;
      }
      if (rv == NGHTTP2_ERR_DEFERRED) {
        DEBUGF("send: frame transmission deferred\n");
        break;
      }
      if (rv < 0) {
        int32_t opened_stream_id = 0;
        uint32_t error_code = NGHTTP2_INTERNAL_ERROR;

        DEBUGF("send: frame preparation failed with %s\n",
               nghttp2_strerror(rv));
        /* TODO If the error comes from compressor, the connection
           must be closed. */
        if (item->frame.hd.type != NGHTTP2_DATA &&
            session->callbacks.on_frame_not_send_callback && is_non_fatal(rv)) {
          nghttp2_frame *frame = &item->frame;
          /* The library is responsible for the transmission of
             WINDOW_UPDATE frame, so we don't call error callback for
             it. */
          if (frame->hd.type != NGHTTP2_WINDOW_UPDATE &&
              session->callbacks.on_frame_not_send_callback(
                  session, frame, rv, session->user_data) != 0) {

            nghttp2_outbound_item_free(item, mem);
            nghttp2_mem_free(mem, item);

            return NGHTTP2_ERR_CALLBACK_FAILURE;
          }
        }
        /* We have to close stream opened by failed request HEADERS
           or PUSH_PROMISE. */
        switch (item->frame.hd.type) {
        case NGHTTP2_HEADERS:
          if (item->frame.headers.cat == NGHTTP2_HCAT_REQUEST) {
            opened_stream_id = item->frame.hd.stream_id;
            if (item->aux_data.headers.canceled) {
              error_code = item->aux_data.headers.error_code;
            } else {
              /* Set error_code to REFUSED_STREAM so that application
                 can send request again. */
              error_code = NGHTTP2_REFUSED_STREAM;
            }
          }
          break;
        case NGHTTP2_PUSH_PROMISE:
          opened_stream_id = item->frame.push_promise.promised_stream_id;
          break;
        }
        if (opened_stream_id) {
          /* careful not to override rv */
          int rv2;
          rv2 = nghttp2_session_close_stream(session, opened_stream_id,
                                             error_code);

          if (nghttp2_is_fatal(rv2)) {
            return rv2;
          }
        }

        nghttp2_outbound_item_free(item, mem);
        nghttp2_mem_free(mem, item);
        active_outbound_item_reset(aob, mem);

        if (rv == NGHTTP2_ERR_HEADER_COMP) {
          /* If header compression error occurred, should terminiate
             connection. */
          rv = nghttp2_session_terminate_session(session,
                                                 NGHTTP2_INTERNAL_ERROR);
        }
        if (nghttp2_is_fatal(rv)) {
          return rv;
        }
        break;
      }

      aob->item = item;

      nghttp2_bufs_rewind(framebufs);

      if (item->frame.hd.type != NGHTTP2_DATA) {
        nghttp2_frame *frame;

        frame = &item->frame;

        DEBUGF("send: next frame: payloadlen=%zu, type=%u, flags=0x%02x, "
               "stream_id=%d\n",
               frame->hd.length, frame->hd.type, frame->hd.flags,
               frame->hd.stream_id);

        rv = session_call_before_frame_send(session, frame);
        if (nghttp2_is_fatal(rv)) {
          return rv;
        }

        if (rv == NGHTTP2_ERR_CANCEL) {
          int32_t opened_stream_id = 0;
          uint32_t error_code = NGHTTP2_INTERNAL_ERROR;

          if (session->callbacks.on_frame_not_send_callback) {
            if (session->callbacks.on_frame_not_send_callback(
                    session, frame, rv, session->user_data) != 0) {
              return NGHTTP2_ERR_CALLBACK_FAILURE;
            }
          }

          /* We have to close stream opened by canceled request
             HEADERS or PUSH_PROMISE. */
          switch (item->frame.hd.type) {
          case NGHTTP2_HEADERS:
            if (item->frame.headers.cat == NGHTTP2_HCAT_REQUEST) {
              opened_stream_id = item->frame.hd.stream_id;
              /* We don't have to check
                 item->aux_data.headers.canceled since it has already
                 been checked. */
              /* Set error_code to REFUSED_STREAM so that application
                 can send request again. */
              error_code = NGHTTP2_REFUSED_STREAM;
            }
            break;
          case NGHTTP2_PUSH_PROMISE:
            opened_stream_id = item->frame.push_promise.promised_stream_id;
            break;
          }
          if (opened_stream_id) {
            /* careful not to override rv */
            int rv2;
            rv2 = nghttp2_session_close_stream(session, opened_stream_id,
                                               error_code);

            if (nghttp2_is_fatal(rv2)) {
              return rv2;
            }
          }

          active_outbound_item_reset(aob, mem);

          break;
        }
      } else {
        DEBUGF("send: next frame: DATA\n");

        if (item->aux_data.data.no_copy) {
          aob->state = NGHTTP2_OB_SEND_NO_COPY;
          break;
        }
      }

      DEBUGF("send: start transmitting frame type=%u, length=%zd\n",
             framebufs->cur->buf.pos[3],
             framebufs->cur->buf.last - framebufs->cur->buf.pos);

      aob->state = NGHTTP2_OB_SEND_DATA;

      break;
    }
    case NGHTTP2_OB_SEND_DATA: {
      size_t datalen;
      nghttp2_buf *buf;

      buf = &framebufs->cur->buf;

      if (buf->pos == buf->last) {
        DEBUGF("send: end transmission of a frame\n");

        /* Frame has completely sent */
        if (fast_cb) {
          rv = session_after_frame_sent2(session);
        } else {
          rv = session_after_frame_sent1(session);
          if (rv < 0) {
            /* FATAL */
            assert(nghttp2_is_fatal(rv));
            return rv;
          }
          rv = session_after_frame_sent2(session);
        }
        if (rv < 0) {
          /* FATAL */
          assert(nghttp2_is_fatal(rv));
          return rv;
        }
        /* We have already adjusted the next state */
        break;
      }

      *data_ptr = buf->pos;
      datalen = nghttp2_buf_len(buf);

      /* We increment the offset here. If send_callback does not send
         everything, we will adjust it. */
      buf->pos += datalen;

      return (ssize_t)datalen;
    }
    case NGHTTP2_OB_SEND_NO_COPY: {
      nghttp2_stream *stream;
      nghttp2_frame *frame;
      int pause;

      DEBUGF("send: no copy DATA\n");

      frame = &aob->item->frame;

      stream = nghttp2_session_get_stream(session, frame->hd.stream_id);
      if (stream == NULL) {
        DEBUGF("send: no copy DATA cancelled because stream was closed\n");

        active_outbound_item_reset(aob, mem);

        break;
      }

      rv = session_call_send_data(session, aob->item, framebufs);
      if (nghttp2_is_fatal(rv)) {
        return rv;
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

        active_outbound_item_reset(aob, mem);

        break;
      }

      if (rv == NGHTTP2_ERR_WOULDBLOCK) {
        return 0;
      }

      pause = (rv == NGHTTP2_ERR_PAUSE);

      rv = session_after_frame_sent1(session);
      if (rv < 0) {
        assert(nghttp2_is_fatal(rv));
        return rv;
      }
      rv = session_after_frame_sent2(session);
      if (rv < 0) {
        assert(nghttp2_is_fatal(rv));
        return rv;
      }

      /* We have already adjusted the next state */

      if (pause) {
        return 0;
      }

      break;
    }
    case NGHTTP2_OB_SEND_CLIENT_MAGIC: {
      size_t datalen;
      nghttp2_buf *buf;

      buf = &framebufs->cur->buf;

      if (buf->pos == buf->last) {
        DEBUGF("send: end transmission of client magic\n");
        active_outbound_item_reset(aob, mem);
        break;
      }

      *data_ptr = buf->pos;
      datalen = nghttp2_buf_len(buf);

      buf->pos += datalen;

      return (ssize_t)datalen;
    }
    }
  }
}