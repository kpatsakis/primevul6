static void ob_q_free(nghttp2_outbound_queue *q, nghttp2_mem *mem) {
  nghttp2_outbound_item *item, *next;
  for (item = q->head; item;) {
    next = item->qnext;
    nghttp2_outbound_item_free(item, mem);
    nghttp2_mem_free(mem, item);
    item = next;
  }
}