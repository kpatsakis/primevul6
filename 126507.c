void mg_error(struct mg_connection *c, const char *fmt, ...) {
  char mem[256], *buf = mem;
  va_list ap;
  va_start(ap, fmt);
  mg_vasprintf(&buf, sizeof(mem), fmt, ap);
  va_end(ap);
  LOG(LL_ERROR, ("%lu %s", c->id, buf));
  c->is_closing = 1;             // Set is_closing before sending MG_EV_CALL
  mg_call(c, MG_EV_ERROR, buf);  // Let user handler to override it
  if (buf != mem) free(buf);
}