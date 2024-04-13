static void test_base64(void) {
  char buf[128];

  ASSERT(mg_base64_encode((uint8_t *) "", 0, buf) == 0);
  ASSERT(strcmp(buf, "") == 0);
  ASSERT(mg_base64_encode((uint8_t *) "x", 1, buf) == 4);
  ASSERT(strcmp(buf, "eA==") == 0);
  ASSERT(mg_base64_encode((uint8_t *) "xyz", 3, buf) == 4);
  ASSERT(strcmp(buf, "eHl6") == 0);
  ASSERT(mg_base64_encode((uint8_t *) "abcdef", 6, buf) == 8);
  ASSERT(strcmp(buf, "YWJjZGVm") == 0);
  ASSERT(mg_base64_encode((uint8_t *) "ы", 2, buf) == 4);
  ASSERT(strcmp(buf, "0Ys=") == 0);
  ASSERT(mg_base64_encode((uint8_t *) "xy", 3, buf) == 4);
  ASSERT(strcmp(buf, "eHkA") == 0);
  ASSERT(mg_base64_encode((uint8_t *) "test", 4, buf) == 8);
  ASSERT(strcmp(buf, "dGVzdA==") == 0);
  ASSERT(mg_base64_encode((uint8_t *) "abcde", 5, buf) == 8);
  ASSERT(strcmp(buf, "YWJjZGU=") == 0);

  ASSERT(mg_base64_decode("кю", 4, buf) == 0);
  ASSERT(mg_base64_decode("A", 1, buf) == 0);
  ASSERT(mg_base64_decode("A=", 2, buf) == 0);
  ASSERT(mg_base64_decode("AA=", 3, buf) == 0);
  ASSERT(mg_base64_decode("AAA=", 4, buf) == 2);
  ASSERT(mg_base64_decode("AAAA====", 8, buf) == 0);
  ASSERT(mg_base64_decode("AAAA----", 8, buf) == 0);
  ASSERT(mg_base64_decode("Q2VzYW50YQ==", 12, buf) == 7);
  ASSERT(strcmp(buf, "Cesanta") == 0);
}