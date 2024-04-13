long mg_tls_send(struct mg_connection *c, const void *buf, size_t len) {
  return c == NULL || buf == NULL || len == 0 ? 0 : -1;
}