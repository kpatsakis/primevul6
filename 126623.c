static bool mg_atonl(struct mg_str str, struct mg_addr *addr) {
  if (mg_vcasecmp(&str, "localhost") != 0) return false;
  addr->ip = mg_htonl(0x7f000001);
  addr->is_ip6 = false;
  return true;
}