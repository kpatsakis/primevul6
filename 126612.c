void mg_ws_upgrade(struct mg_connection *c, struct mg_http_message *hm,
                   const char *fmt, ...) {
  struct mg_str *wskey = mg_http_get_header(hm, "Sec-WebSocket-Key");
  c->pfn = mg_ws_cb;
  c->pfn_data = NULL;
  if (wskey == NULL) {
    mg_http_reply(c, 426, "", "WS upgrade expected\n");
    c->is_draining = 1;
  } else {
    struct mg_str *wsproto = mg_http_get_header(hm, "Sec-WebSocket-Protocol");
    va_list ap;
    va_start(ap, fmt);
    ws_handshake(c, wskey, wsproto, fmt, ap);
    va_end(ap);
    c->is_websocket = 1;
    mg_call(c, MG_EV_WS_OPEN, hm);
  }
}