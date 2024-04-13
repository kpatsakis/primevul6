void mg_http_write_chunk(struct mg_connection *c, const char *buf, size_t len) {
  mg_printf(c, "%lX\r\n", (unsigned long) len);
  mg_send(c, buf, len);
  mg_send(c, "\r\n", 2);
}