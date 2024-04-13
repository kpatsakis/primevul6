struct mg_str mg_str_s(const char *s) {
  struct mg_str str = {s, s == NULL ? 0 : strlen(s)};
  return str;
}