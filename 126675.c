static void test_get_header_var(void) {
  struct mg_str empty = mg_str(""), bar = mg_str("bar"), baz = mg_str("baz");
  struct mg_str header = mg_str("Digest foo=\"bar\", blah,boo=baz, x=\"yy\"");
  struct mg_str yy = mg_str("yy");
  // struct mg_str x = mg_http_get_header_var(header, mg_str("x"));
  // LOG(LL_INFO, ("--> [%d] [%d]", (int) x.len, yy.len));
  ASSERT(mg_strcmp(empty, mg_http_get_header_var(empty, empty)) == 0);
  ASSERT(mg_strcmp(empty, mg_http_get_header_var(header, empty)) == 0);
  ASSERT(mg_strcmp(empty, mg_http_get_header_var(header, mg_str("fooo"))) == 0);
  ASSERT(mg_strcmp(empty, mg_http_get_header_var(header, mg_str("fo"))) == 0);
  ASSERT(mg_strcmp(empty, mg_http_get_header_var(header, mg_str("blah"))) == 0);
  ASSERT(mg_strcmp(bar, mg_http_get_header_var(header, mg_str("foo"))) == 0);
  ASSERT(mg_strcmp(baz, mg_http_get_header_var(header, mg_str("boo"))) == 0);
  ASSERT(mg_strcmp(yy, mg_http_get_header_var(header, mg_str("x"))) == 0);
}