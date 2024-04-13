static void test_http_chunked(void) {
  struct mg_mgr mgr;
  const char *data, *url = "http://127.0.0.1:12344";
  uint32_t i, done = 0;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, url, eh2, NULL);

  mg_http_connect(&mgr, url, eh3, &done);
  for (i = 0; i < 50 && done == 0; i++) mg_mgr_poll(&mgr, 1);
  ASSERT(i < 50);
  data = LONG_CHUNK "chunk 1chunk 2";
  ASSERT(done == mg_crc32(0, data, strlen(data)));

  done = 0;
  mg_http_connect(&mgr, url, eh4, &done);
  for (i = 0; i < 50 && done == 0; i++) mg_mgr_poll(&mgr, 1);
  data = LONG_CHUNK LONG_CHUNK "chunk 1chunk 2" LONG_CHUNK "chunk 1chunk 2";
  ASSERT(done == mg_crc32(0, data, strlen(data)));

  done = 0;
  mg_http_connect(&mgr, url, eh5, &done);
  for (i = 0; i < 50 && done == 0; i++) mg_mgr_poll(&mgr, 1);
  data = LONG_CHUNK "chunk 1chunk 2";
  ASSERT(done == mg_crc32(0, data, strlen(data)));

  mg_mgr_free(&mgr);
  ASSERT(mgr.conns == NULL);
}