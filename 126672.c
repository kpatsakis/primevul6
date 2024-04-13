static void eh6(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_READ) *(int *) fn_data = 1;
  (void) c, (void) ev_data;
}