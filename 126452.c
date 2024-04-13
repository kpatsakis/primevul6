static void eh1(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  struct mg_tls_opts *topts = (struct mg_tls_opts *) fn_data;
  if (ev == MG_EV_ACCEPT && topts != NULL) mg_tls_init(c, topts);
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    LOG(LL_INFO,
        ("[%.*s %.*s] message len %d", (int) hm->method.len, hm->method.ptr,
         (int) hm->uri.len, hm->uri.ptr, (int) hm->message.len));
    if (mg_http_match_uri(hm, "/foo/*")) {
      mg_http_reply(c, 200, "", "uri: %.*s", hm->uri.len - 5, hm->uri.ptr + 5);
    } else if (mg_http_match_uri(hm, "/ws")) {
      mg_ws_upgrade(c, hm, NULL);
    } else if (mg_http_match_uri(hm, "/body")) {
      mg_http_reply(c, 200, "", "%.*s", (int) hm->body.len, hm->body.ptr);
    } else if (mg_http_match_uri(hm, "/bar")) {
      mg_http_reply(c, 404, "", "not found");
    } else if (mg_http_match_uri(hm, "/no_reason")) {
      mg_printf(c, "%s", "HTTP/1.0 200\r\nContent-Length: 2\r\n\r\nok");
    } else if (mg_http_match_uri(hm, "/badroot")) {
      struct mg_http_serve_opts sopts;
      memset(&sopts, 0, sizeof(sopts));
      sopts.root_dir = "/BAAADDD!";
      mg_http_serve_dir(c, hm, &sopts);
    } else if (mg_http_match_uri(hm, "/creds")) {
      char user[100], pass[100];
      mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));
      mg_http_reply(c, 200, "", "[%s]:[%s]", user, pass);
    } else if (mg_http_match_uri(hm, "/upload")) {
      mg_http_upload(c, hm, ".");
    } else if (mg_http_match_uri(hm, "/test/")) {
      struct mg_http_serve_opts sopts;
      memset(&sopts, 0, sizeof(sopts));
      sopts.root_dir = ".";
      sopts.extra_headers = "A: B\r\nC: D\r\n";
      mg_http_serve_dir(c, hm, &sopts);
    } else if (mg_http_match_uri(hm, "/servefile")) {
      struct mg_http_serve_opts sopts;
      memset(&sopts, 0, sizeof(sopts));
      sopts.mime_types = "foo=a/b,txt=c/d";
      mg_http_serve_file(c, hm, "test/data/a.txt", &sopts);
    } else {
      struct mg_http_serve_opts sopts;
      memset(&sopts, 0, sizeof(sopts));
      sopts.root_dir = "./test/data";
      sopts.ssi_pattern = "#.shtml";
      sopts.extra_headers = "C: D\r\n";
      mg_http_serve_dir(c, hm, &sopts);
    }
  } else if (ev == MG_EV_WS_OPEN) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    ASSERT(mg_strcmp(hm->uri, mg_str("/ws")) == 0);
    mg_ws_send(c, "opened", 6, WEBSOCKET_OP_BINARY);
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    mg_ws_send(c, wm->data.ptr, wm->data.len, WEBSOCKET_OP_BINARY);
  }
}