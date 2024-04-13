static void test_rewrites(void) {
  char buf[FETCH_BUF_SIZE];
  const char *url = "http://LOCALHOST:12358";
  const char *expected = "#define MG_VERSION \"" MG_VERSION "\"\n";
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  ASSERT(mg_http_listen(&mgr, url, h7, NULL) != NULL);
  ASSERT(fetch(&mgr, buf, url, "GET /a.txt HTTP/1.0\n\n") == 200);
  ASSERT(cmpbody(buf, "hello\n") == 0);
  ASSERT(fetch(&mgr, buf, url, "GET /foo/version.h HTTP/1.0\n\n") == 200);
  ASSERT(cmpbody(buf, expected) == 0);
  ASSERT(fetch(&mgr, buf, url, "GET /foo HTTP/1.0\n\n") == 301);
  ASSERT(fetch(&mgr, buf, url, "GET /foo/ HTTP/1.0\n\n") == 200);
  // printf("-->[%s]\n", buf);
  // exit(0);
  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}