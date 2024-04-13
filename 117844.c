int nghttp2_session_reprioritize_stream(
    nghttp2_session *session, nghttp2_stream *stream,
    const nghttp2_priority_spec *pri_spec_in) {
  int rv;
  nghttp2_stream *dep_stream = NULL;
  nghttp2_priority_spec pri_spec_default;
  const nghttp2_priority_spec *pri_spec = pri_spec_in;

  assert(pri_spec->stream_id != stream->stream_id);

  if (!nghttp2_stream_in_dep_tree(stream)) {
    return 0;
  }

  if (pri_spec->stream_id != 0) {
    dep_stream = nghttp2_session_get_stream_raw(session, pri_spec->stream_id);

    if (!dep_stream &&
        session_detect_idle_stream(session, pri_spec->stream_id)) {

      nghttp2_priority_spec_default_init(&pri_spec_default);

      dep_stream = nghttp2_session_open_stream(
          session, pri_spec->stream_id, NGHTTP2_FLAG_NONE, &pri_spec_default,
          NGHTTP2_STREAM_IDLE, NULL);

      if (dep_stream == NULL) {
        return NGHTTP2_ERR_NOMEM;
      }
    } else if (!dep_stream || !nghttp2_stream_in_dep_tree(dep_stream)) {
      nghttp2_priority_spec_default_init(&pri_spec_default);
      pri_spec = &pri_spec_default;
    }
  }

  if (pri_spec->stream_id == 0) {
    dep_stream = &session->root;
  } else if (nghttp2_stream_dep_find_ancestor(dep_stream, stream)) {
    DEBUGF("stream: cycle detected, dep_stream(%p)=%d stream(%p)=%d\n",
           dep_stream, dep_stream->stream_id, stream, stream->stream_id);

    nghttp2_stream_dep_remove_subtree(dep_stream);
    rv = nghttp2_stream_dep_add_subtree(stream->dep_prev, dep_stream);
    if (rv != 0) {
      return rv;
    }
  }

  assert(dep_stream);

  if (dep_stream == stream->dep_prev && !pri_spec->exclusive) {
    /* This is minor optimization when just weight is changed. */
    nghttp2_stream_change_weight(stream, pri_spec->weight);

    return 0;
  }

  nghttp2_stream_dep_remove_subtree(stream);

  /* We have to update weight after removing stream from tree */
  stream->weight = pri_spec->weight;

  if (pri_spec->exclusive) {
    rv = nghttp2_stream_dep_insert_subtree(dep_stream, stream);
  } else {
    rv = nghttp2_stream_dep_add_subtree(dep_stream, stream);
  }

  if (rv != 0) {
    return rv;
  }

  return 0;
}