int mg_base64_decode(const char *src, int n, char *dst) {
  const char *end = src + n;
  int len = 0;
  while (src + 3 < end) {
    int a = mg_b64rev(src[0]), b = mg_b64rev(src[1]), c = mg_b64rev(src[2]),
        d = mg_b64rev(src[3]);
    if (a == 64 || a < 0 || b == 64 || b < 0 || c < 0 || d < 0) return 0;
    dst[len++] = (char) ((a << 2) | (b >> 4));
    if (src[2] != '=') {
      dst[len++] = (char) ((b << 4) | (c >> 2));
      if (src[3] != '=') dst[len++] = (char) ((c << 6) | d);
    }
    src += 4;
  }
  dst[len] = '\0';
  return len;
}