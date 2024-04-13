char *mg_hexdump(const void *buf, size_t len) {
  const unsigned char *p = (const unsigned char *) buf;
  size_t i, idx, n = 0, ofs = 0, dlen = len * 5 + 100;
  char ascii[17] = "", *dst = (char *) calloc(1, dlen);
  if (dst == NULL) return dst;
  for (i = 0; i < len; i++) {
    idx = i % 16;
    if (idx == 0) {
      if (i > 0 && dlen > n)
        n += (size_t) snprintf(dst + n, dlen - n, "  %s\n", ascii);
      if (dlen > n)
        n += (size_t) snprintf(dst + n, dlen - n, "%04x ", (int) (i + ofs));
    }
    if (dlen < n) break;
    n += (size_t) snprintf(dst + n, dlen - n, " %02x", p[i]);
    ascii[idx] = (char) (p[i] < 0x20 || p[i] > 0x7e ? '.' : p[i]);
    ascii[idx + 1] = '\0';
  }
  while (i++ % 16) {
    if (n < dlen) n += (size_t) snprintf(dst + n, dlen - n, "%s", "   ");
  }
  if (n < dlen) n += (size_t) snprintf(dst + n, dlen - n, "  %s\n", ascii);
  if (n > dlen - 1) n = dlen - 1;
  dst[n] = '\0';
  return dst;
}