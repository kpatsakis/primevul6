static void f3(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  int *ok = (int *) fn_data;
  // LOG(LL_INFO, ("%d", ev));
  if (ev == MG_EV_CONNECT) {
    // c->is_hexdumping = 1;
    mg_printf(c, "GET / HTTP/1.0\r\nHost: %s\r\n\r\n",
              c->peer.is_ip6 ? "ipv6.google.com" : "cesanta.com");
  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    // LOG(LL_INFO, ("-->[%.*s]", (int) hm->message.len, hm->message.ptr));
    // ASSERT(mg_vcmp(&hm->method, "HTTP/1.1") == 0);
    // ASSERT(mg_vcmp(&hm->uri, "301") == 0);
    *ok = atoi(hm->uri.ptr);
  } else if (ev == MG_EV_CLOSE) {
    if (*ok == 0) *ok = 888;
  } else if (ev == MG_EV_ERROR) {
    if (*ok == 0) *ok = 777;
  }
}