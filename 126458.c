bool mg_file_printf(const char *path, const char *fmt, ...) {
  char tmp[256], *buf = tmp;
  bool result;
  int len;
  va_list ap;
  va_start(ap, fmt);
  len = mg_vasprintf(&buf, sizeof(tmp), fmt, ap);
  va_end(ap);
  result = mg_file_write(path, buf, len > 0 ? (size_t) len : 0);
  if (buf != tmp) free(buf);
  return result;
}