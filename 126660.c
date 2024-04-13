static void test_dns_timeout(const char *dns_server_url, const char *errstr) {
  // Test timeout
  struct mg_mgr mgr;
  char buf[100] = "";
  int i;
  mg_mgr_init(&mgr);
  mgr.dns4.url = dns_server_url;
  mgr.dnstimeout = 10;
  LOG(LL_DEBUG, ("opening dummy DNS listener..."));
  mg_listen(&mgr, mgr.dns4.url, NULL, NULL);  // Just discard our queries
  mg_http_connect(&mgr, "http://google.com", fn1, buf);
  for (i = 0; i < 50 && buf[0] == '\0'; i++) mg_mgr_poll(&mgr, 1);
  mg_mgr_free(&mgr);
  LOG(LL_DEBUG, ("buf: [%s]", buf));
  ASSERT(strcmp(buf, errstr) == 0);
}