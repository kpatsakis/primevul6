struct mg_str mg_url_host(const char *url) {
  struct url u = urlparse(url);
  size_t n = u.port  ? u.port - u.host - 1
             : u.uri ? u.uri - u.host
                     : u.end - u.host;
  struct mg_str s = mg_str_n(url + u.host, n);
  return s;
}