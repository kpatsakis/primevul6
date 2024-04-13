static void mg_http_vprintf_chunk(struct mg_connection *c, const char *fmt,
                                  va_list ap) {
  char mem[256], *buf = mem;
  int len = mg_vasprintf(&buf, sizeof(mem), fmt, ap);
  mg_printf(c, "%X\r\n", len);
  mg_send(c, buf, len > 0 ? (size_t) len : 0);
  mg_send(c, "\r\n", 2);
  if (buf != mem) free(buf);
}