static void test_tls(void) {
#if MG_ENABLE_MBEDTLS || MG_ENABLE_OPENSSL
  struct mg_tls_opts opts = {.ca = "./test/data/ss_ca.pem",
                             .cert = "./test/data/ss_server.pem",
                             .certkey = "./test/data/ss_server.pem"};
  struct mg_mgr mgr;
  struct mg_connection *c;
  const char *url = "https://127.0.0.1:12347";
  char buf[FETCH_BUF_SIZE];
  mg_mgr_init(&mgr);
  c = mg_http_listen(&mgr, url, eh1, (void *) &opts);
  ASSERT(c != NULL);
  ASSERT(fetch(&mgr, buf, url, "GET /a.txt HTTP/1.0\n\n") == 200);
  // LOG(LL_INFO, ("%s", buf));
  ASSERT(cmpbody(buf, "hello\n") == 0);
  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
#endif
}