struct mg_connection *mg_mkpipe(struct mg_mgr *mgr, mg_event_handler_t fn,
                                void *fn_data) {
  union usa usa[2];
  SOCKET sp[2] = {INVALID_SOCKET, INVALID_SOCKET};
  struct mg_connection *c = NULL;
  if (!mg_socketpair(sp, usa)) {
    LOG(LL_ERROR, ("Cannot create socket pair"));
  } else if ((c = alloc_conn(mgr, false, sp[1])) == NULL) {
    closesocket(sp[0]);
    closesocket(sp[1]);
    LOG(LL_ERROR, ("OOM"));
  } else {
    LOG(LL_INFO, ("pipe %lu", (unsigned long) sp[0]));
    tomgaddr(&usa[0], &c->peer, false);
    c->is_udp = 1;
    c->pfn = pf1;
    c->pfn_data = (void *) (size_t) sp[0];
    c->fn = fn;
    c->fn_data = fn_data;
    mg_call(c, MG_EV_OPEN, NULL);
    LIST_ADD_HEAD(struct mg_connection, &mgr->conns, c);
  }
  return c;
}