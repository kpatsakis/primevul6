static void test_http_client(void) {
  struct mg_mgr mgr;
  struct mg_connection *c;
  int i, ok = 0;
  mg_mgr_init(&mgr);
  c = mg_http_connect(&mgr, "http://cesanta.com", f3, &ok);
  ASSERT(c != NULL);
  for (i = 0; i < 500 && ok <= 0; i++) mg_mgr_poll(&mgr, 10);
  ASSERT(ok == 301);
  c->is_closing = 1;
  mg_mgr_poll(&mgr, 0);
  ok = 0;
#if MG_ENABLE_MBEDTLS || MG_ENABLE_OPENSSL
  {
    struct mg_tls_opts opts = {.ca = "./test/data/ca.pem"};
    c = mg_http_connect(&mgr, "https://cesanta.com", f3, &ok);
    ASSERT(c != NULL);
    mg_tls_init(c, &opts);
    for (i = 0; i < 500 && ok <= 0; i++) mg_mgr_poll(&mgr, 10);
    ASSERT(ok == 200);
  }
#endif

#if MG_ENABLE_IPV6
  ok = 0;
  // ipv6.google.com does not have IPv4 address, only IPv6, therefore
  // it is guaranteed to hit IPv6 resolution path.
  c = mg_http_connect(&mgr, "http://ipv6.google.com", f3, &ok);
  ASSERT(c != NULL);
  for (i = 0; i < 500 && ok <= 0; i++) mg_mgr_poll(&mgr, 10);
  ASSERT(ok == 200);
#endif

  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}