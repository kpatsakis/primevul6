struct mg_str mg_url_user(const char *url) {
  struct url u = urlparse(url);
  struct mg_str s = mg_str("");
  if (u.user && (u.pass || u.host)) {
    size_t n = u.pass ? u.pass - u.user - 1 : u.host - u.user - 1;
    s = mg_str_n(url + u.user, n);
  }
  return s;
}