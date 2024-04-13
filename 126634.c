static void ws_handshake(struct mg_connection *c, const struct mg_str *wskey,
                         const struct mg_str *wsproto, const char *fmt,
                         va_list ap) {
  const char *magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  unsigned char sha[20], b64_sha[30];
  char mem[128], *buf = mem;

  mg_sha1_ctx sha_ctx;
  mg_sha1_init(&sha_ctx);
  mg_sha1_update(&sha_ctx, (unsigned char *) wskey->ptr, wskey->len);
  mg_sha1_update(&sha_ctx, (unsigned char *) magic, 36);
  mg_sha1_final(sha, &sha_ctx);
  mg_base64_encode(sha, sizeof(sha), (char *) b64_sha);
  buf[0] = '\0';
  if (fmt != NULL) mg_vasprintf(&buf, sizeof(mem), fmt, ap);
  mg_printf(c,
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "%s",
            b64_sha, buf);
  if (buf != mem) free(buf);
  if (wsproto != NULL) {
    mg_printf(c, "Sec-WebSocket-Protocol: %.*s\r\n", (int) wsproto->len,
              wsproto->ptr);
  }
  mg_send(c, "\r\n", 2);
}