static void test_pipe(void) {
  struct mg_mgr mgr;
  struct mg_connection *c;
  int i, done = 0;
  mg_mgr_init(&mgr);
  ASSERT((c = mg_mkpipe(&mgr, eh6, (void *) &done)) != NULL);
  mg_mgr_wakeup(c);
  for (i = 0; i < 10 && done == 0; i++) mg_mgr_poll(&mgr, 1);
  ASSERT(done == 1);
  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}