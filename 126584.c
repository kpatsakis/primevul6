static int fetch(struct mg_mgr *mgr, char *buf, const char *url,
                 const char *fmt, ...) {
  struct fetch_data fd = {buf, 0, 0};
  int i;
  struct mg_connection *c = mg_http_connect(mgr, url, fcb, &fd);
  va_list ap;
  ASSERT(c != NULL);
  if (mg_url_is_ssl(url)) {
    struct mg_tls_opts opts;
    struct mg_str host = mg_url_host(url);
    memset(&opts, 0, sizeof(opts));
    opts.ca = "./test/data/ca.pem";
    if (strstr(url, "127.0.0.1") != NULL) {
      // Local connection, use self-signed certificates
      opts.ca = "./test/data/ss_ca.pem";
      opts.cert = "./test/data/ss_client.pem";
    } else {
      opts.srvname = host;
    }
    mg_tls_init(c, &opts);
    // c->is_hexdumping = 1;
  }
  va_start(ap, fmt);
  mg_vprintf(c, fmt, ap);
  va_end(ap);
  buf[0] = '\0';
  for (i = 0; i < 250 && buf[0] == '\0'; i++) mg_mgr_poll(mgr, 1);
  if (!fd.closed) c->is_closing = 1;
  mg_mgr_poll(mgr, 1);
  return fd.code;
}