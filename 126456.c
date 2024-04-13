void mg_log(const char *fmt, ...) {
  char mem[256], *buf = mem;
  va_list ap;
  int len = 0;
  va_start(ap, fmt);
  len = mg_vasprintf(&buf, sizeof(mem), fmt, ap);
  va_end(ap);
  s_fn(buf, len > 0 ? (size_t) len : 0, s_fn_param);
  s_fn("\n", 1, s_fn_param);
  if (buf != mem) free(buf);
}