struct mg_connection *mg_ws_connect(struct mg_mgr *mgr, const char *url,
                                    mg_event_handler_t fn, void *fn_data,
                                    const char *fmt, ...) {
  struct mg_connection *c = mg_connect(mgr, url, fn, fn_data);
  if (c != NULL) {
    char nonce[16], key[30], mem1[128], mem2[256], *buf1 = mem1, *buf2 = mem2;
    struct mg_str host = mg_url_host(url);
    int n1 = 0, n2 = 0;
    if (fmt != NULL) {
      va_list ap;
      va_start(ap, fmt);
      n1 = mg_vasprintf(&buf1, sizeof(mem1), fmt, ap);
      va_end(ap);
    }
    // Send handshake request
    // mg_url_host(url, host, sizeof(host));
    mg_random(nonce, sizeof(nonce));
    mg_base64_encode((unsigned char *) nonce, sizeof(nonce), key);
    // LOG(LL_DEBUG, "%p [%s]", uri, uri == NULL ? "???" : uri);
    n2 = mg_asprintf(&buf2, sizeof(mem2),
                     "GET %s HTTP/1.1\r\n"
                     "Upgrade: websocket\r\n"
                     "Host: %.*s\r\n"
                     "Connection: Upgrade\r\n"
                     "%.*s"
                     "Sec-WebSocket-Version: 13\r\n"
                     "Sec-WebSocket-Key: %s\r\n"
                     "\r\n",
                     mg_url_uri(url), (int) host.len, host.ptr, n1, buf1, key);
    mg_send(c, buf2, n2 > 0 ? (size_t) n2 : 0);
    if (buf1 != mem1) free(buf1);
    if (buf2 != mem2) free(buf2);
    c->pfn = mg_ws_cb;
    c->pfn_data = NULL;
  }
  return c;
}