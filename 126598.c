static struct mg_connection *alloc_conn(struct mg_mgr *mgr, bool is_client,
                                        SOCKET fd) {
  struct mg_connection *c = (struct mg_connection *) calloc(1, sizeof(*c));
  if (c != NULL) {
    c->is_client = is_client;
    c->fd = S2PTR(fd);
    c->mgr = mgr;
    c->id = ++mgr->nextid;
  }
  return c;
}