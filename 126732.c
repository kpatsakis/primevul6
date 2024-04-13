static void test_str(void) {
  struct mg_str s = mg_strdup(mg_str("a"));
  ASSERT(mg_strcmp(s, mg_str("a")) == 0);
  free((void *) s.ptr);
  ASSERT(mg_strcmp(mg_str(""), mg_str(NULL)) == 0);
  ASSERT(mg_strcmp(mg_str("a"), mg_str("b")) < 0);
  ASSERT(mg_strcmp(mg_str("b"), mg_str("a")) > 0);
  ASSERT(mg_strstr(mg_str("abc"), mg_str("d")) == NULL);
  ASSERT(mg_strstr(mg_str("abc"), mg_str("b")) != NULL);
  ASSERT(mg_strcmp(mg_str("hi"), mg_strstrip(mg_str(" \thi\r\n"))) == 0);
}