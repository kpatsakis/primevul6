struct mg_connection *mg_listen(struct mg_mgr *mgr, const char *url,
                                mg_event_handler_t fn, void *fn_data) {
  struct mg_connection *c = NULL;
  bool is_udp = strncmp(url, "udp:", 4) == 0;
  struct mg_addr addr;
  SOCKET fd = mg_open_listener(url, &addr);
  if (fd == INVALID_SOCKET) {
    LOG(LL_ERROR, ("Failed: %s, errno %d", url, MG_SOCK_ERRNO));
  } else if ((c = alloc_conn(mgr, 0, fd)) == NULL) {
    LOG(LL_ERROR, ("OOM %s", url));
    closesocket(fd);
  } else {
    memcpy(&c->peer, &addr, sizeof(struct mg_addr));
    c->fd = S2PTR(fd);
    c->is_listening = 1;
    c->is_udp = is_udp;
    LIST_ADD_HEAD(struct mg_connection, &mgr->conns, c);
    c->fn = fn;
    c->fn_data = fn_data;
    mg_call(c, MG_EV_OPEN, NULL);
    LOG(LL_DEBUG,
        ("%lu accepting on %s (port %u)", c->id, url, mg_ntohs(c->peer.port)));
  }
  return c;
}