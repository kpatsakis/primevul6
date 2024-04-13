static void u1(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_CONNECT) {
    ((int *) fn_data)[0] += 1;
    mg_send(c, "hi", 2);
  } else if (ev == MG_EV_WRITE) {
    ((int *) fn_data)[0] += 100;
  } else if (ev == MG_EV_READ) {
    ((int *) fn_data)[0] += 10;
    mg_iobuf_free(&c->recv);
  }
  (void) ev_data;
}