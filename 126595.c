static void sntp_cb(struct mg_connection *c, int ev, void *evd, void *fnd) {
  if (ev == MG_EV_READ) {
    int64_t milliseconds = mg_sntp_parse(c->recv.buf, c->recv.len);
    if (milliseconds > 0) {
      mg_call(c, MG_EV_SNTP_TIME, &milliseconds);
      LOG(LL_DEBUG, ("%u.%u, next at %lu", (unsigned) (milliseconds / 1000),
                     (unsigned) (milliseconds % 1000), s_sntmp_next));
    }
    c->recv.len = 0;  // Clear receive buffer
  } else if (ev == MG_EV_CONNECT) {
    mg_sntp_send(c, (unsigned long) time(NULL));
  } else if (ev == MG_EV_CLOSE) {
  }
  (void) fnd;
  (void) evd;
}