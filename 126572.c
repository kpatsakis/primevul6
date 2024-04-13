long mg_tls_recv(struct mg_connection *c, void *buf, size_t len) {
  struct mg_tls *tls = (struct mg_tls *) c->tls;
  int n = SSL_read(tls->ssl, buf, (int) len);
  return n == 0 ? -1 : n < 0 && mg_tls_err(tls, n) == 0 ? 0 : n;
}