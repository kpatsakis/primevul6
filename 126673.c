static void test_udp(void) {
  struct mg_mgr mgr;
  const char *url = "udp://127.0.0.1:12353";
  int i, done = 0;
  mg_mgr_init(&mgr);
  mg_listen(&mgr, url, u1, (void *) &done);
  mg_connect(&mgr, url, u1, (void *) &done);
  for (i = 0; i < 5; i++) mg_mgr_poll(&mgr, 1);
  // LOG(LL_INFO, ("%d", done));
  ASSERT(done == 111);
  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}