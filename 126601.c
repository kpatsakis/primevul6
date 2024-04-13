static void http_cb(struct mg_connection *c, int ev, void *evd, void *fnd) {
  if (ev == MG_EV_READ || ev == MG_EV_CLOSE) {
    struct mg_http_message hm;
    while (c->recv.buf != NULL && c->recv.len > 0) {
      int n = mg_http_parse((char *) c->recv.buf, c->recv.len, &hm);
      bool is_chunked = n > 0 && mg_is_chunked(&hm);
      if (ev == MG_EV_CLOSE) {
        hm.message.len = c->recv.len;
        hm.body.len = hm.message.len - (size_t) (hm.body.ptr - hm.message.ptr);
      } else if (is_chunked && n > 0) {
        walkchunks(c, &hm, (size_t) n);
      }
      // LOG(LL_INFO,
      //("---->%d %d\n%.*s", n, is_chunked, (int) c->recv.len, c->recv.buf));
      if (n < 0 && ev == MG_EV_READ) {
        mg_error(c, "HTTP parse:\n%.*s", (int) c->recv.len, c->recv.buf);
        break;
      } else if (n > 0 && (size_t) c->recv.len >= hm.message.len) {
        mg_call(c, MG_EV_HTTP_MSG, &hm);
        mg_iobuf_del(&c->recv, 0, hm.message.len);
      } else {
        if (n > 0 && !is_chunked) {
          hm.chunk =
              mg_str_n((char *) &c->recv.buf[n], c->recv.len - (size_t) n);
          mg_call(c, MG_EV_HTTP_CHUNK, &hm);
        }
        break;
      }
    }
  }
  (void) fnd;
  (void) evd;
}