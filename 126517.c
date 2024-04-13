unsigned short mg_url_port(const char *url) {
  struct url u = urlparse(url);
  unsigned short port = 0;
  if (strncmp(url, "http:", 5) == 0 || strncmp(url, "ws:", 3) == 0) port = 80;
  if (strncmp(url, "wss:", 4) == 0 || strncmp(url, "https:", 6) == 0)
    port = 443;
  if (strncmp(url, "mqtt:", 5) == 0) port = 1883;
  if (strncmp(url, "mqtts:", 6) == 0) port = 8883;
  if (u.port) port = (unsigned short) atoi(url + u.port);
  return port;
}