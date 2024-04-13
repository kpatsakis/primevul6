static void test_http_range(void) {
  struct mg_mgr mgr;
  const char *url = "http://127.0.0.1:12349";
  struct mg_http_message hm;
  char buf[FETCH_BUF_SIZE];

  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, url, ehr, NULL);

  ASSERT(fetch(&mgr, buf, url, "GET /range.txt HTTP/1.0\n\n") == 200);
  ASSERT(mg_http_parse(buf, strlen(buf), &hm) > 0);
  ASSERT(hm.body.len == 312);

  fetch(&mgr, buf, url, "%s", "GET /range.txt HTTP/1.0\nRange: bytes=5-10\n\n");
  ASSERT(mg_http_parse(buf, strlen(buf), &hm) > 0);
  ASSERT(mg_strcmp(hm.uri, mg_str("206")) == 0);
  ASSERT(mg_strcmp(hm.proto, mg_str("Partial Content")) == 0);
  ASSERT(mg_strcmp(hm.body, mg_str(" of co")) == 0);
  ASSERT(mg_strcmp(*mg_http_get_header(&hm, "Content-Range"),
                   mg_str("bytes 5-10/312")) == 0);

  // Fetch till EOF
  fetch(&mgr, buf, url, "%s", "GET /range.txt HTTP/1.0\nRange: bytes=300-\n\n");
  ASSERT(mg_http_parse(buf, strlen(buf), &hm) > 0);
  ASSERT(mg_strcmp(hm.uri, mg_str("206")) == 0);
  ASSERT(mg_strcmp(hm.body, mg_str("is disease.\n")) == 0);
  // LOG(LL_INFO, ("----%d\n[%s]", (int) hm.body.len, buf));

  // Fetch past EOF, must trigger 416 response
  fetch(&mgr, buf, url, "%s", "GET /range.txt HTTP/1.0\nRange: bytes=999-\n\n");
  ASSERT(mg_http_parse(buf, strlen(buf), &hm) > 0);
  ASSERT(mg_strcmp(hm.uri, mg_str("416")) == 0);
  ASSERT(hm.body.len == 0);
  ASSERT(mg_strcmp(*mg_http_get_header(&hm, "Content-Range"),
                   mg_str("bytes */312")) == 0);

  fetch(&mgr, buf, url, "%s",
        "GET /range.txt HTTP/1.0\nRange: bytes=0-312\n\n");
  ASSERT(mg_http_parse(buf, strlen(buf), &hm) > 0);
  ASSERT(mg_strcmp(hm.uri, mg_str("416")) == 0);

  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}