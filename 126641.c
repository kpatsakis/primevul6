static void test_http_get_var(void) {
  char buf[256];
  struct mg_str body;
  body = mg_str("key1=value1&key2=value2&key3=value%203&key4=value+4");
  ASSERT(mg_http_get_var(&body, "key1", buf, sizeof(buf)) == 6);
  ASSERT(strcmp(buf, "value1") == 0);
  ASSERT(mg_http_get_var(&body, "KEY1", buf, sizeof(buf)) == 6);
  ASSERT(strcmp(buf, "value1") == 0);
  ASSERT(mg_http_get_var(&body, "key2", buf, sizeof(buf)) == 6);
  ASSERT(strcmp(buf, "value2") == 0);
  ASSERT(mg_http_get_var(&body, "key3", buf, sizeof(buf)) == 7);
  ASSERT(strcmp(buf, "value 3") == 0);
  ASSERT(mg_http_get_var(&body, "key4", buf, sizeof(buf)) == 7);
  ASSERT(strcmp(buf, "value 4") == 0);

  ASSERT(mg_http_get_var(&body, "key", buf, sizeof(buf)) == -4);
  ASSERT(mg_http_get_var(&body, "key1", NULL, sizeof(buf)) == -2);
  ASSERT(mg_http_get_var(&body, "key1", buf, 0) == -2);
  ASSERT(mg_http_get_var(&body, NULL, buf, sizeof(buf)) == -1);
  ASSERT(mg_http_get_var(&body, "key1", buf, 1) == -3);

  body = mg_str("key=broken%2");
  ASSERT(mg_http_get_var(&body, "key", buf, sizeof(buf)) == -3);

  body = mg_str("key=broken%2x");
  ASSERT(mg_http_get_var(&body, "key", buf, sizeof(buf)) == -3);
  ASSERT(mg_http_get_var(&body, "inexistent", buf, sizeof(buf)) == -4);
  body = mg_str("key=%");
  ASSERT(mg_http_get_var(&body, "key", buf, sizeof(buf)) == -3);
  body = mg_str("&&&kEy=%");
  ASSERT(mg_http_get_var(&body, "kEy", buf, sizeof(buf)) == -3);
}