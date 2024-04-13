void mg_http_reply(struct mg_connection *c, int code, const char *headers,
                   const char *fmt, ...) {
  char mem[256], *buf = mem;
  va_list ap;
  int len;
  va_start(ap, fmt);
  len = mg_vasprintf(&buf, sizeof(mem), fmt, ap);
  va_end(ap);
  mg_printf(c, "HTTP/1.1 %d %s\r\n%sContent-Length: %d\r\n\r\n", code,
            mg_http_status_code_str(code), headers == NULL ? "" : headers, len);
  mg_send(c, buf, len > 0 ? (size_t) len : 0);
  if (buf != mem) free(buf);
}