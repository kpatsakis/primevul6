static bool mg_v4mapped(struct mg_str str, struct mg_addr *addr) {
  int i;
  if (str.len < 14) return false;
  if (str.ptr[0] != ':' || str.ptr[1] != ':' || str.ptr[6] != ':') return false;
  for (i = 2; i < 6; i++) {
    if (str.ptr[i] != 'f' && str.ptr[i] != 'F') return false;
  }
  if (!mg_aton4(mg_str_n(&str.ptr[7], str.len - 7), addr)) return false;
  memset(addr->ip6, 0, sizeof(addr->ip6));
  addr->ip6[10] = addr->ip6[11] = 255;
  memcpy(&addr->ip6[12], &addr->ip, 4);
  addr->is_ip6 = true;
  return true;
}