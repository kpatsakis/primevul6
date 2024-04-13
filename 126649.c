void mg_call(struct mg_connection *c, int ev, void *ev_data) {
  // Run user-defined handler first, in order to give it an ability
  // to intercept processing (e.g. clean input buffer) before the
  // protocol handler kicks in
  if (c->fn != NULL) c->fn(c, ev, ev_data, c->fn_data);
  if (c->pfn != NULL) c->pfn(c, ev, ev_data, c->pfn_data);
}