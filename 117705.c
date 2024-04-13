static int free_streams(nghttp2_map_entry *entry, void *ptr) {
  nghttp2_session *session;
  nghttp2_stream *stream;
  nghttp2_outbound_item *item;
  nghttp2_mem *mem;

  session = (nghttp2_session *)ptr;
  mem = &session->mem;
  stream = (nghttp2_stream *)entry;
  item = stream->item;

  if (item && !item->queued && item != session->aob.item) {
    nghttp2_outbound_item_free(item, mem);
    nghttp2_mem_free(mem, item);
  }

  nghttp2_stream_free(stream);
  nghttp2_mem_free(mem, stream);

  return 0;
}