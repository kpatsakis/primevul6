static void test_packed(void) {
  struct mg_mgr mgr;
  const char *url = "http://127.0.0.1:12351";
  char buf[FETCH_BUF_SIZE] = "", *data = mg_file_read("Makefile", NULL);
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, url, eh7, NULL);

  // Load top level file directly
  ASSERT(fetch(&mgr, buf, url, "GET /Makefile HTTP/1.0\n\n") == 200);
  ASSERT(cmpbody(buf, data) == 0);
  free(data);

  // Load file deeper in the FS tree directly
  data = mg_file_read("src/ssi.h", NULL);
  ASSERT(fetch(&mgr, buf, url, "GET /src/ssi.h HTTP/1.0\n\n") == 200);
  ASSERT(cmpbody(buf, data) == 0);
  free(data);

  // List root dir
  ASSERT(fetch(&mgr, buf, url, "GET / HTTP/1.0\n\n") == 200);
  // printf("--------\n%s\n", buf);

  // List nested dir
  ASSERT(fetch(&mgr, buf, url, "GET /test HTTP/1.0\n\n") == 301);
  ASSERT(fetch(&mgr, buf, url, "GET /test/ HTTP/1.0\n\n") == 200);
  // printf("--------\n%s\n", buf);

  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}