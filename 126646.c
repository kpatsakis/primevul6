static void mg_ws_cb(struct mg_connection *c, int ev, void *ev_data,
                     void *fn_data) {
  struct ws_msg msg;
  size_t ofs = (size_t) c->pfn_data;

  // assert(ofs < c->recv.len);
  if (ev == MG_EV_READ) {
    if (!c->is_websocket && c->is_client) {
      int n = mg_http_get_request_len(c->recv.buf, c->recv.len);
      if (n < 0) {
        c->is_closing = 1;  // Some just, not an HTTP request
      } else if (n > 0) {
        if (n < 15 || memcmp(c->recv.buf + 9, "101", 3) != 0) {
          LOG(LL_ERROR,
              ("%lu WS handshake error: %.*s", c->id, 15, c->recv.buf));
          c->is_closing = 1;
        } else {
          struct mg_http_message hm;
          mg_http_parse((char *) c->recv.buf, c->recv.len, &hm);
          c->is_websocket = 1;
          mg_call(c, MG_EV_WS_OPEN, &hm);
        }
        mg_iobuf_del(&c->recv, 0, (size_t) n);
      } else {
        return;  // A request is not yet received
      }
    }

    while (ws_process(c->recv.buf + ofs, c->recv.len - ofs, &msg) > 0) {
      char *s = (char *) c->recv.buf + ofs + msg.header_len;
      struct mg_ws_message m = {{s, msg.data_len}, msg.flags};
      size_t len = msg.header_len + msg.data_len;
      uint8_t final = msg.flags & 128, op = msg.flags & 15;
      // LOG(LL_VERBOSE_DEBUG, ("fin %d op %d len %d [%.*s]", final, op,
      //                       (int) m.data.len, (int) m.data.len, m.data.ptr));
      switch (op) {
        case WEBSOCKET_OP_CONTINUE:
          mg_call(c, MG_EV_WS_CTL, &m);
          break;
        case WEBSOCKET_OP_PING:
          LOG(LL_DEBUG, ("%s", "WS PONG"));
          mg_ws_send(c, s, msg.data_len, WEBSOCKET_OP_PONG);
          mg_call(c, MG_EV_WS_CTL, &m);
          break;
        case WEBSOCKET_OP_PONG:
          mg_call(c, MG_EV_WS_CTL, &m);
          break;
        case WEBSOCKET_OP_TEXT:
        case WEBSOCKET_OP_BINARY:
          if (final) mg_call(c, MG_EV_WS_MSG, &m);
          break;
        case WEBSOCKET_OP_CLOSE:
          LOG(LL_DEBUG, ("%lu Got WS CLOSE", c->id));
          mg_call(c, MG_EV_WS_CTL, &m);
          c->is_closing = 1;
          break;
        default:
          // Per RFC6455, close conn when an unknown op is recvd
          mg_error(c, "unknown WS op %d", op);
          break;
      }

      // Handle fragmented frames: strip header, keep in c->recv
      if (final == 0 || op == 0) {
        if (op) ofs++, len--, msg.header_len--;       // First frame
        mg_iobuf_del(&c->recv, ofs, msg.header_len);  // Strip header
        len -= msg.header_len;
        ofs += len;
        c->pfn_data = (void *) ofs;
        // LOG(LL_INFO, ("FRAG %d [%.*s]", (int) ofs, (int) ofs, c->recv.buf));
      }
      // Remove non-fragmented frame
      if (final && op) mg_iobuf_del(&c->recv, ofs, len);
      // Last chunk of the fragmented frame
      if (final && !op) {
        m.flags = c->recv.buf[0];
        m.data = mg_str_n((char *) &c->recv.buf[1], (size_t) (ofs - 1));
        mg_call(c, MG_EV_WS_MSG, &m);
        mg_iobuf_del(&c->recv, 0, ofs);
        ofs = 0;
        c->pfn_data = NULL;
      }
    }
  }
  (void) fn_data;
  (void) ev_data;
}