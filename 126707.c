static int cmpbody(const char *buf, const char *str) {
  struct mg_http_message hm;
  struct mg_str s = mg_str(str);
  size_t len = strlen(buf);
  mg_http_parse(buf, len, &hm);
  if (hm.body.len > len) hm.body.len = len - (size_t) (hm.body.ptr - buf);
  return mg_strcmp(hm.body, s);
}