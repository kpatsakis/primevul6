int mg_vprintf(struct mg_connection *c, const char *fmt, va_list ap) {
  char mem[256], *buf = mem;
  int len = mg_vasprintf(&buf, sizeof(mem), fmt, ap);
  len = mg_send(c, buf, len > 0 ? (size_t) len : 0);
  if (buf != mem) free(buf);
  return len;
}