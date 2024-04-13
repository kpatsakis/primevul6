char *mg_hex(const void *buf, size_t len, char *to) {
  const unsigned char *p = (const unsigned char *) buf;
  static const char *hex = "0123456789abcdef";
  size_t i = 0;
  for (; len--; p++) {
    to[i++] = hex[p[0] >> 4];
    to[i++] = hex[p[0] & 0x0f];
  }
  to[i] = '\0';
  return to;
}