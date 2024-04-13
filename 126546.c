static bool mg_atone(struct mg_str str, struct mg_addr *addr) {
  if (str.len > 0) return false;
  addr->ip = 0;
  addr->is_ip6 = false;
  return true;
}