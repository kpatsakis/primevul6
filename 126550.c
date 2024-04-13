static void f5(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    mg_printf(c, "HTTP/1.0 200 OK\n\n%.*s", (int) hm->uri.len, hm->uri.ptr);
    (*(int *) fn_data)++;
  }
}