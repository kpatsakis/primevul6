void mg_http_bauth(struct mg_connection *c, const char *user,
                   const char *pass) {
  struct mg_str u = mg_str(user), p = mg_str(pass);
  size_t need = c->send.len + 36 + (u.len + p.len) * 2;
  if (c->send.size < need) mg_iobuf_resize(&c->send, need);
  if (c->send.size >= need) {
    int i, n = 0;
    char *buf = (char *) &c->send.buf[c->send.len + 21];
    memcpy(&buf[-21], "Authorization: Basic ", 21);  // DON'T use mg_send!
    for (i = 0; i < (int) u.len; i++) {
      n = mg_base64_update(((unsigned char *) u.ptr)[i], buf, n);
    }
    if (p.len > 0) {
      n = mg_base64_update(':', buf, n);
      for (i = 0; i < (int) p.len; i++) {
        n = mg_base64_update(((unsigned char *) p.ptr)[i], buf, n);
      }
    }
    n = mg_base64_final(buf, n);
    c->send.len += 21 + (size_t) n + 2;
    memcpy(&c->send.buf[c->send.len - 2], "\r\n", 2);
  } else {
    LOG(LL_ERROR, ("%lu %s cannot resize iobuf %d->%d ", c->id, c->label,
                   (int) c->send.size, (int) need));
  }
}