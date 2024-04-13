nghttp2_stream *nghttp2_session_open_stream(nghttp2_session *session,
                                            int32_t stream_id, uint8_t flags,
                                            nghttp2_priority_spec *pri_spec_in,
                                            nghttp2_stream_state initial_state,
                                            void *stream_user_data) {
  int rv;
  nghttp2_stream *stream;
  nghttp2_stream *dep_stream = NULL;
  int stream_alloc = 0;
  nghttp2_priority_spec pri_spec_default;
  nghttp2_priority_spec *pri_spec = pri_spec_in;
  nghttp2_mem *mem;

  mem = &session->mem;
  stream = nghttp2_session_get_stream_raw(session, stream_id);

  if (stream) {
    assert(stream->state == NGHTTP2_STREAM_IDLE);
    assert(nghttp2_stream_in_dep_tree(stream));
    nghttp2_session_detach_idle_stream(session, stream);
    rv = nghttp2_stream_dep_remove(stream);
    if (rv != 0) {
      return NULL;
    }
  } else {
    stream = nghttp2_mem_malloc(mem, sizeof(nghttp2_stream));
    if (stream == NULL) {
      return NULL;
    }

    stream_alloc = 1;
  }

  if (pri_spec->stream_id != 0) {
    dep_stream = nghttp2_session_get_stream_raw(session, pri_spec->stream_id);

    if (!dep_stream &&
        session_detect_idle_stream(session, pri_spec->stream_id)) {
      /* Depends on idle stream, which does not exist in memory.
         Assign default priority for it. */
      nghttp2_priority_spec_default_init(&pri_spec_default);

      dep_stream = nghttp2_session_open_stream(
          session, pri_spec->stream_id, NGHTTP2_FLAG_NONE, &pri_spec_default,
          NGHTTP2_STREAM_IDLE, NULL);

      if (dep_stream == NULL) {
        if (stream_alloc) {
          nghttp2_mem_free(mem, stream);
        }

        return NULL;
      }
    } else if (!dep_stream || !nghttp2_stream_in_dep_tree(dep_stream)) {
      /* If dep_stream is not part of dependency tree, stream will get
         default priority.  This handles the case when
         pri_spec->stream_id == stream_id.  This happens because we
         don't check pri_spec->stream_id against new stream ID in
         nghttp2_submit_request.  This also handles the case when idle
         stream created by PRIORITY frame was opened.  Somehow we
         first remove the idle stream from dependency tree.  This is
         done to simplify code base, but ideally we should retain old
         dependency.  But I'm not sure this adds values. */
      nghttp2_priority_spec_default_init(&pri_spec_default);
      pri_spec = &pri_spec_default;
    }
  }

  if (initial_state == NGHTTP2_STREAM_RESERVED) {
    flags |= NGHTTP2_STREAM_FLAG_PUSH;
  }

  if (stream_alloc) {
    nghttp2_stream_init(stream, stream_id, flags, initial_state,
                        pri_spec->weight,
                        (int32_t)session->remote_settings.initial_window_size,
                        (int32_t)session->local_settings.initial_window_size,
                        stream_user_data, mem);

    rv = nghttp2_map_insert(&session->streams, &stream->map_entry);
    if (rv != 0) {
      nghttp2_stream_free(stream);
      nghttp2_mem_free(mem, stream);
      return NULL;
    }
  } else {
    stream->flags = flags;
    stream->state = initial_state;
    stream->weight = pri_spec->weight;
    stream->stream_user_data = stream_user_data;
  }

  switch (initial_state) {
  case NGHTTP2_STREAM_RESERVED:
    if (nghttp2_session_is_my_stream_id(session, stream_id)) {
      /* reserved (local) */
      nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_RD);
    } else {
      /* reserved (remote) */
      nghttp2_stream_shutdown(stream, NGHTTP2_SHUT_WR);
      ++session->num_incoming_reserved_streams;
    }
    /* Reserved stream does not count in the concurrent streams
       limit. That is one of the DOS vector. */
    break;
  case NGHTTP2_STREAM_IDLE:
    /* Idle stream does not count toward the concurrent streams limit.
       This is used as anchor node in dependency tree. */
    nghttp2_session_keep_idle_stream(session, stream);
    break;
  default:
    if (nghttp2_session_is_my_stream_id(session, stream_id)) {
      ++session->num_outgoing_streams;
    } else {
      ++session->num_incoming_streams;
    }
  }

  if (pri_spec->stream_id == 0) {
    dep_stream = &session->root;
  }

  assert(dep_stream);

  if (pri_spec->exclusive) {
    rv = nghttp2_stream_dep_insert(dep_stream, stream);
    if (rv != 0) {
      return NULL;
    }
  } else {
    nghttp2_stream_dep_add(dep_stream, stream);
  }

  return stream;
}