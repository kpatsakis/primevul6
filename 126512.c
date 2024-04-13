static void test_http_no_content_length(void) {
  struct mg_mgr mgr;
  const char *url = "http://127.0.0.1:12348";
  char buf[FETCH_BUF_SIZE];
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, url, f4, (void *) "baz");
  ASSERT(fetch(&mgr, buf, url, "GET /foo/bar HTTP/1.0\r\n\n") == 200);
  ASSERT(cmpbody(buf, "/foo/bar/baz") == 0);
  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}