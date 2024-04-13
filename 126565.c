static void w2(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  struct mg_str msg = mg_str_n("lebowski", 8);
  if (ev == MG_EV_HTTP_MSG) {
    mg_ws_upgrade(c, (struct mg_http_message *) ev_data, NULL);
  } else if (ev == MG_EV_WS_OPEN) {
    mg_ws_send(c, "x", 1, WEBSOCKET_OP_PONG);
  } else if (ev == MG_EV_POLL && c->is_websocket) {
    size_t ofs, n = (size_t) fn_data;
    if (n < msg.len) {
      // Send "msg" char by char using fragmented frames
      // mg_ws_send() sets the FIN flag in the WS header. Clean it
      // to send fragmented packet. Insert PONG messages between frames
      uint8_t op = n == 0 ? WEBSOCKET_OP_TEXT : WEBSOCKET_OP_CONTINUE;
      mg_ws_send(c, ":->", 3, WEBSOCKET_OP_PING);
      ofs = c->send.len;
      mg_ws_send(c, &msg.ptr[n], 1, op);
      if (n < msg.len - 1) c->send.buf[ofs] = op;  // Clear FIN flag
      c->fn_data = (void *) (n + 1);               // Point to the next char
    } else {
      mg_ws_send(c, "", 0, WEBSOCKET_OP_CLOSE);
    }
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    ASSERT(mg_strcmp(wm->data, mg_str("hi there!")) == 0);
  }
}