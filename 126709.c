static void test_ws_fragmentation(void) {
  const char *url = "ws://localhost:12357/ws";
  struct mg_mgr mgr;
  int i, done = 0;

  mg_mgr_init(&mgr);
  ASSERT(mg_http_listen(&mgr, url, w2, NULL) != NULL);
  mg_ws_connect(&mgr, url, w3, &done, "%s", "Sec-WebSocket-Protocol: echo\r\n");
  for (i = 0; i < 25; i++) mg_mgr_poll(&mgr, 1);
  // LOG(LL_INFO, ("--> %d", done));
  ASSERT(done == 11);

  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}