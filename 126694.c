struct mg_str mg_http_get_header_var(struct mg_str s, struct mg_str v) {
  size_t i;
  for (i = 0; v.len > 0 && i + v.len + 2 < s.len; i++) {
    if (s.ptr[i + v.len] == '=' && memcmp(&s.ptr[i], v.ptr, v.len) == 0) {
      const char *p = &s.ptr[i + v.len + 1], *b = p, *x = &s.ptr[s.len];
      int q = p < x && *p == '"' ? 1 : 0;
      while (p < x &&
             (q ? p == b || *p != '"' : *p != ';' && *p != ' ' && *p != ','))
        p++;
      // LOG(LL_INFO, ("[%.*s] [%.*s] [%.*s]", (int) s.len, s.ptr, (int) v.len,
      // v.ptr, (int) (p - b), b));
      return stripquotes(mg_str_n(b, (size_t) (p - b + q)));
    }
  }
  return mg_str_n(NULL, 0);
}