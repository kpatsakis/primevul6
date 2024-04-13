const char *mg_url_uri(const char *url) {
  struct url u = urlparse(url);
  return u.uri ? url + u.uri : "/";
}