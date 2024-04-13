int mg_http_get_var(const struct mg_str *buf, const char *name, char *dst,
                    size_t dst_len) {
  const char *p, *e, *s;
  size_t name_len;
  int len;

  if (dst == NULL || dst_len == 0) {
    len = -2;  // Bad destination
  } else if (buf->ptr == NULL || name == NULL || buf->len == 0) {
    len = -1;  // Bad source
    dst[0] = '\0';
  } else {
    name_len = strlen(name);
    e = buf->ptr + buf->len;
    len = -4;  // Name does not exist
    dst[0] = '\0';
    for (p = buf->ptr; p + name_len < e; p++) {
      if ((p == buf->ptr || p[-1] == '&') && p[name_len] == '=' &&
          !mg_ncasecmp(name, p, name_len)) {
        p += name_len + 1;
        s = (const char *) memchr(p, '&', (size_t) (e - p));
        if (s == NULL) s = e;
        len = mg_url_decode(p, (size_t) (s - p), dst, dst_len, 1);
        if (len < 0) len = -3;  // Failed to decode
        break;
      }
    }
  }
  return len;
}