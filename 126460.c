int mg_printf(struct mg_connection *c, const char *fmt, ...) {
  int len = 0;
  va_list ap;
  va_start(ap, fmt);
  len = mg_vprintf(c, fmt, ap);
  va_end(ap);
  return len;
}