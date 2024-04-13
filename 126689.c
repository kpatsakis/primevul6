int mg_url_is_ssl(const char *url) {
  return strncmp(url, "wss:", 4) == 0 || strncmp(url, "https:", 6) == 0 ||
         strncmp(url, "mqtts:", 6) == 0 || strncmp(url, "ssl:", 4) == 0 ||
         strncmp(url, "tls:", 4) == 0;
}