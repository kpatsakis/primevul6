int mg_asprintf(char **buf, size_t size, const char *fmt, ...) {
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = mg_vasprintf(buf, size, fmt, ap);
  va_end(ap);
  return ret;
}