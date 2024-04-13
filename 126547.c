static void fn1(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_ERROR) sprintf((char *) fn_data, "%s", (char *) ev_data);
  (void) c;
}