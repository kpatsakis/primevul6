static void wcb(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_WS_OPEN) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    struct mg_str *wsproto = mg_http_get_header(hm, "Sec-WebSocket-Protocol");
    ASSERT(wsproto != NULL);
    mg_ws_send(c, "boo", 3, WEBSOCKET_OP_BINARY);
    mg_ws_send(c, "", 0, WEBSOCKET_OP_PING);
    ((int *) fn_data)[0] += 100;
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    if (mg_strstr(wm->data, mg_str("boo")))
      mg_ws_send(c, "", 0, WEBSOCKET_OP_CLOSE);
    ((int *) fn_data)[0]++;
  } else if (ev == MG_EV_CLOSE) {
    ((int *) fn_data)[0] += 10;
  }
}