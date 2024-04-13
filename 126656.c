unsigned long mg_unhexn(const char *s, size_t len) {
  unsigned long i = 0, v = 0;
  for (i = 0; i < len; i++) v <<= 4, v |= mg_unhex_nimble(((uint8_t *) s)[i]);
  return v;
}