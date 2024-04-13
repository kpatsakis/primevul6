int mg_base64_encode(const unsigned char *p, int n, char *to) {
  int i, len = 0;
  for (i = 0; i < n; i++) len = mg_base64_update(p[i], to, len);
  len = mg_base64_final(to, len);
  return len;
}