void nghttp2_session_del(nghttp2_session *session) {
  nghttp2_mem *mem;
  nghttp2_inflight_settings *settings;

  if (session == NULL) {
    return;
  }

  mem = &session->mem;

  for (settings = session->inflight_settings_head; settings;) {
    nghttp2_inflight_settings *next = settings->next;
    inflight_settings_del(settings, mem);
    settings = next;
  }

  nghttp2_stream_free(&session->root);

  /* Have to free streams first, so that we can check
     stream->item->queued */
  nghttp2_map_each_free(&session->streams, free_streams, session);
  nghttp2_map_free(&session->streams);

  ob_q_free(&session->ob_urgent, mem);
  ob_q_free(&session->ob_reg, mem);
  ob_q_free(&session->ob_syn, mem);

  active_outbound_item_reset(&session->aob, mem);
  session_inbound_frame_reset(session);
  nghttp2_hd_deflate_free(&session->hd_deflater);
  nghttp2_hd_inflate_free(&session->hd_inflater);
  nghttp2_bufs_free(&session->aob.framebufs);
  nghttp2_mem_free(mem, session);
}