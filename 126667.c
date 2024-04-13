static struct url urlparse(const char *url) {
  size_t i;
  struct url u;
  memset(&u, 0, sizeof(u));
  for (i = 0; url[i] != '\0'; i++) {
    if (i > 0 && u.host == 0 && url[i - 1] == '/' && url[i] == '/') {
      u.host = i + 1;
      u.port = 0;
    } else if (url[i] == ']') {
      u.port = 0;  // IPv6 URLs, like http://[::1]/bar
    } else if (url[i] == ':' && u.port == 0 && u.uri == 0) {
      u.port = i + 1;
    } else if (url[i] == '@' && u.user == 0 && u.pass == 0) {
      u.user = u.host;
      u.pass = u.port;
      u.host = i + 1;
      u.port = 0;
    } else if (u.host && u.uri == 0 && url[i] == '/') {
      u.uri = i;
    }
  }
  u.end = i;
#if 0
  printf("[%s] %d %d %d %d %d\n", url, u.user, u.pass, u.host, u.port, u.uri);
#endif
  return u;
}