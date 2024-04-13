char *mg_straddr(struct mg_addr *a, char *buf, size_t len) {
  char tmp[30];
  const char *fmt = a->is_ip6 ? "[%s]:%d" : "%s:%d";
  mg_ntoa(a, tmp, sizeof(tmp));
  snprintf(buf, len, fmt, tmp, (int) mg_ntohs(a->port));
  return buf;
}