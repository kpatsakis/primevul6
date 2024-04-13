static void sntp_cb(struct mg_connection *c, int ev, void *evd, void *fnd) {
  if (ev == MG_EV_SNTP_TIME) {
    *(int64_t *) fnd = *(int64_t *) evd;
  }
  (void) c;
}