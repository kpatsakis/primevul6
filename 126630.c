static void eh3(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_CONNECT) {
    mg_printf(c, "GET / HTTP/1.0\n\n");
  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    // LOG(LL_INFO, ("----> [%.*s]", (int) hm->body.len, hm->body.ptr));
    c->is_closing = 1;
    *(uint32_t *) fn_data = mg_crc32(0, hm->body.ptr, hm->body.len);
  }
}