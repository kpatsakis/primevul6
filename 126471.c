static void test_multipart(void) {
  struct mg_http_part part;
  size_t ofs;
  const char *s =
      "--xyz\r\n"
      "Content-Disposition: form-data; name=\"val\"\r\n"
      "\r\n"
      "abc\r\ndef\r\n"
      "--xyz\r\n"
      "Content-Disposition: form-data; name=\"foo\"; filename=\"a b.txt\"\r\n"
      "Content-Type: text/plain\r\n"
      "\r\n"
      "hello world\r\n"
      "\r\n"
      "--xyz--\r\n";
  ASSERT(mg_http_next_multipart(mg_str(""), 0, NULL) == 0);
  ASSERT((ofs = mg_http_next_multipart(mg_str(s), 0, &part)) > 0);
  ASSERT(mg_strcmp(part.name, mg_str("val")) == 0);
  // LOG(LL_INFO, ("--> [%.*s]", (int) part.body.len, part.body.ptr));
  ASSERT(mg_strcmp(part.body, mg_str("abc\r\ndef")) == 0);
  ASSERT(part.filename.len == 0);
  ASSERT((ofs = mg_http_next_multipart(mg_str(s), ofs, &part)) > 0);
  ASSERT(mg_strcmp(part.name, mg_str("foo")) == 0);
  // LOG(LL_INFO, ("--> [%.*s]", (int) part.filename.len, part.filename.ptr));
  ASSERT(mg_strcmp(part.filename, mg_str("a b.txt")) == 0);
  ASSERT(mg_strcmp(part.body, mg_str("hello world\r\n")) == 0);
  ASSERT(mg_http_next_multipart(mg_str(s), ofs, &part) == 0);
}