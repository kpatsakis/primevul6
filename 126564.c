long mg_tls_send(struct mg_connection *c, const void *buf, size_t len) {
  struct mg_tls *tls = (struct mg_tls *) c->tls;
  int n = SSL_write(tls->ssl, buf, (int) len);
  return n == 0 ? -1 : n < 0 && mg_tls_err(tls, n) == 0 ? 0 : n;
}