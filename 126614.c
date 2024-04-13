static void eh7(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    struct mg_http_serve_opts sopts;
    memset(&sopts, 0, sizeof(sopts));
    sopts.root_dir = "";
    sopts.fs = &mg_fs_packed;
    mg_http_serve_dir(c, hm, &sopts);
  }
  (void) ev_data, (void) fn_data;
}