static void test_commalist(void) {
  struct mg_str k, v, s1 = mg_str(""), s2 = mg_str("a"), s3 = mg_str("a,b");
  struct mg_str s4 = mg_str("a=123"), s5 = mg_str("a,b=123");
  ASSERT(mg_commalist(&s1, &k, &v) == false);

  ASSERT(mg_commalist(&s2, &k, &v) == true);
  ASSERT(v.len == 0 && mg_vcmp(&k, "a") == 0);
  ASSERT(mg_commalist(&s2, &k, &v) == false);

  ASSERT(mg_commalist(&s3, &k, &v) == true);
  ASSERT(v.len == 0 && mg_vcmp(&k, "a") == 0);
  ASSERT(mg_commalist(&s3, &k, &v) == true);
  ASSERT(v.len == 0 && mg_vcmp(&k, "b") == 0);
  ASSERT(mg_commalist(&s3, &k, &v) == false);

  ASSERT(mg_commalist(&s4, &k, &v) == true);
  ASSERT(mg_vcmp(&k, "a") == 0 && mg_vcmp(&v, "123") == 0);
  ASSERT(mg_commalist(&s4, &k, &v) == false);
  ASSERT(mg_commalist(&s4, &k, &v) == false);

  ASSERT(mg_commalist(&s5, &k, &v) == true);
  ASSERT(v.len == 0 && mg_vcmp(&k, "a") == 0);
  ASSERT(mg_commalist(&s5, &k, &v) == true);
  ASSERT(mg_vcmp(&k, "b") == 0 && mg_vcmp(&v, "123") == 0);
  ASSERT(mg_commalist(&s4, &k, &v) == false);
}