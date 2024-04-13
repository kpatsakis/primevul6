static void f4(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    mg_printf(c, "HTTP/1.0 200 OK\n\n%.*s/%s", (int) hm->uri.len, hm->uri.ptr,
              fn_data);
    c->is_draining = 1;
  }
}