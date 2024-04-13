static void eh2(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    mg_printf(c, "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    mg_http_printf_chunk(c, LONG_CHUNK);
    c->label[0] = 1;
  } else if (ev == MG_EV_POLL) {
    if (c->label[0] > 0 && c->send.len == 0) c->label[0]++;
    if (c->label[0] > 10 && c->label[0] != 'x') {
      mg_http_printf_chunk(c, "chunk 1");
      mg_http_printf_chunk(c, "chunk 2");
      mg_http_printf_chunk(c, "");
      c->label[0] = 'x';
    }
  }
  (void) ev_data;
  (void) fn_data;
}