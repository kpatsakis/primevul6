static void eh4(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  uint32_t *crc = (uint32_t *) c->label;
  if (ev == MG_EV_CONNECT) {
    mg_printf(c, "GET / HTTP/1.0\n\n");
  } else if (ev == MG_EV_HTTP_CHUNK) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    *crc = mg_crc32(*crc, hm->chunk.ptr, hm->chunk.len);
  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    *crc = mg_crc32(*crc, hm->body.ptr, hm->body.len);
    // LOG(LL_INFO, ("MSG [%.*s]", (int) hm->body.len, hm->body.ptr));
    c->is_closing = 1;
    *(uint32_t *) fn_data = *crc;
  }
  (void) ev_data;
}