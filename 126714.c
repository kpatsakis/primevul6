void mg_mgr_wakeup(struct mg_connection *c) {
  LOG(LL_INFO, ("skt: %p", c->pfn_data));
  send((SOCKET) (size_t) c->pfn_data, "\x01", 1, MSG_NONBLOCKING);
}