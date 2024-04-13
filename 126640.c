char *mg_ntoa(const struct mg_addr *addr, char *buf, size_t len) {
  if (addr->is_ip6) {
    uint16_t *p = (uint16_t *) addr->ip6;
    snprintf(buf, len, "%hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx", mg_htons(p[0]),
             mg_htons(p[1]), mg_htons(p[2]), mg_htons(p[3]), mg_htons(p[4]),
             mg_htons(p[5]), mg_htons(p[6]), mg_htons(p[7]));
  } else {
    uint8_t p[4];
    memcpy(p, &addr->ip, sizeof(p));
    snprintf(buf, len, "%d.%d.%d.%d", (int) p[0], (int) p[1], (int) p[2],
             (int) p[3]);
  }
  return buf;
}