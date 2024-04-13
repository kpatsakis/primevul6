long mg_tls_send(struct mg_connection *c, const void *buf, size_t len) {
  struct mg_tls *tls = (struct mg_tls *) c->tls;
  long n = mbedtls_ssl_write(&tls->ssl, (unsigned char *) buf, len);
  return n == 0 ? -1 : n == MBEDTLS_ERR_SSL_WANT_WRITE ? 0 : n;
}