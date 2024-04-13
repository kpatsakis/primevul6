struct mg_str mg_strdup(const struct mg_str s) {
  struct mg_str r = {NULL, 0};
  if (s.len > 0 && s.ptr != NULL) {
    char *sc = (char *) calloc(1, s.len + 1);
    if (sc != NULL) {
      memcpy(sc, s.ptr, s.len);
      sc[s.len] = '\0';
      r.ptr = sc;
      r.len = s.len;
    }
  }
  return r;
}