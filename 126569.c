struct mg_connection *mg_mqtt_connect(struct mg_mgr *mgr, const char *url,
                                      struct mg_mqtt_opts *opts,
                                      mg_event_handler_t fn, void *fn_data) {
  struct mg_connection *c = mg_connect(mgr, url, fn, fn_data);
  if (c != NULL) {
    struct mg_mqtt_opts empty;
    memset(&empty, 0, sizeof(empty));
    mg_mqtt_login(c, opts == NULL ? &empty : opts);
    c->pfn = mqtt_cb;
  }
  return c;
}