static void active_outbound_item_reset(nghttp2_active_outbound_item *aob,
                                       nghttp2_mem *mem) {
  DEBUGF("send: reset nghttp2_active_outbound_item\n");
  DEBUGF("send: aob->item = %p\n", aob->item);
  nghttp2_outbound_item_free(aob->item, mem);
  nghttp2_mem_free(mem, aob->item);
  aob->item = NULL;
  nghttp2_bufs_reset(&aob->framebufs);
  aob->state = NGHTTP2_OB_POP_ITEM;
}