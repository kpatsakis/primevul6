struct mg_str mg_url_pass(const char *url) {
  struct url u = urlparse(url);
  struct mg_str s = mg_str_n("", 0UL);
  if (u.pass && u.host) {
    size_t n = u.host - u.pass - 1;
    s = mg_str_n(url + u.pass, n);
  }
  return s;
}