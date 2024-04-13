bool mg_aton(struct mg_str str, struct mg_addr *addr) {
  // LOG(LL_INFO, ("[%.*s]", (int) str.len, str.ptr));
  return mg_atone(str, addr) || mg_atonl(str, addr) || mg_aton4(str, addr) ||
         mg_aton6(str, addr);
}