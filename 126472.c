int mg_check_ip_acl(struct mg_str acl, uint32_t remote_ip) {
  struct mg_str k, v;
  int allowed = acl.len == 0 ? '+' : '-';  // If any ACL is set, deny by default
  while (mg_commalist(&acl, &k, &v)) {
    uint32_t net, mask;
    if (k.ptr[0] != '+' && k.ptr[0] != '-') return -1;
    if (parse_net(&k.ptr[1], &net, &mask) == 0) return -2;
    if ((remote_ip & mask) == net) allowed = k.ptr[0];
  }
  return allowed == '+';
}