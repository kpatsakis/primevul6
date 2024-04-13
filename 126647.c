static bool cmpheader(const char *buf, const char *name, const char *value) {
  struct mg_http_message hm;
  struct mg_str *h;
  size_t len = strlen(buf);
  mg_http_parse(buf, len, &hm);
  h = mg_http_get_header(&hm, name);
  return h != NULL && mg_strcmp(*h, mg_str(value)) == 0;
}