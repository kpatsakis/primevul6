void mg_tls_init(struct mg_connection *c, struct mg_tls_opts *opts) {
  (void) opts;
  mg_error(c, "TLS is not enabled");
}