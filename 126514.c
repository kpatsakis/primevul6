static void test_ws(void) {
  char buf[FETCH_BUF_SIZE];
  const char *url = "ws://LOCALHOST:12343/ws";
  struct mg_mgr mgr;
  int i, done = 0;

  mg_mgr_init(&mgr);
  ASSERT(mg_http_listen(&mgr, url, eh1, NULL) != NULL);
  mg_ws_connect(&mgr, url, wcb, &done, "%s", "Sec-WebSocket-Protocol: meh\r\n");
  for (i = 0; i < 30; i++) mg_mgr_poll(&mgr, 1);
  // LOG(LL_INFO, ("--> %d", done));
  ASSERT(done == 112);

  // Test that non-WS requests fail
  ASSERT(fetch(&mgr, buf, url, "GET /ws HTTP/1.0\r\n\n") == 426);

  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}