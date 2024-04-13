static void pf1(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_READ) mg_iobuf_free(&c->recv);
  (void) ev_data, (void) fn_data;
}