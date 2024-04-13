static void fcb(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    struct fetch_data *fd = (struct fetch_data *) fn_data;
    snprintf(fd->buf, FETCH_BUF_SIZE, "%.*s", (int) hm->message.len,
             hm->message.ptr);
    fd->code = atoi(hm->uri.ptr);
    fd->closed = 1;
    c->is_closing = 1;
    (void) c;
  }
}