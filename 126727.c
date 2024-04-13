static int getrange(struct mg_str *s, int64_t *a, int64_t *b) {
  size_t i, numparsed = 0;
  LOG(LL_INFO, ("%.*s", (int) s->len, s->ptr));
  for (i = 0; i + 6 < s->len; i++) {
    if (memcmp(&s->ptr[i], "bytes=", 6) == 0) {
      struct mg_str p = mg_str_n(s->ptr + i + 6, s->len - i - 6);
      if (p.len > 0 && p.ptr[0] >= '0' && p.ptr[0] <= '9') numparsed++;
      *a = mg_to64(p);
      // LOG(LL_INFO, ("PPP [%.*s] %d", (int) p.len, p.ptr, numparsed));
      while (p.len && p.ptr[0] >= '0' && p.ptr[0] <= '9') p.ptr++, p.len--;
      if (p.len && p.ptr[0] == '-') p.ptr++, p.len--;
      *b = mg_to64(p);
      if (p.len > 0 && p.ptr[0] >= '0' && p.ptr[0] <= '9') numparsed++;
      // LOG(LL_INFO, ("PPP [%.*s] %d", (int) p.len, p.ptr, numparsed));
      break;
    }
  }
  return (int) numparsed;
}