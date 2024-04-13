struct mg_connection *mg_connect(struct mg_mgr *mgr, const char *url,
                                 mg_event_handler_t fn, void *fn_data) {
  struct mg_connection *c = NULL;
  if (url == NULL || url[0] == '\0') {
    LOG(LL_ERROR, ("null url"));
  } else if ((c = alloc_conn(mgr, 1, INVALID_SOCKET)) == NULL) {
    LOG(LL_ERROR, ("OOM"));
  } else {
    LIST_ADD_HEAD(struct mg_connection, &mgr->conns, c);
    c->is_udp = (strncmp(url, "udp:", 4) == 0);
    c->fn = fn;
    c->fn_data = fn_data;
    LOG(LL_DEBUG, ("%lu -> %s", c->id, url));
    mg_call(c, MG_EV_OPEN, NULL);
    mg_resolve(c, url);
  }
  return c;
}