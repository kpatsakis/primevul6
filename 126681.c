static void eh9(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_ERROR) {
    ASSERT(!strcmp((char *) ev_data, "error connecting to 127.0.0.1:55117"));
    *(int *) fn_data = 7;
  }
  (void) c;
}