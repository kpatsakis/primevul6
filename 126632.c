struct mg_str mg_str_n(const char *s, size_t n) {
  struct mg_str str = {s, n};
  return str;
}