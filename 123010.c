  void trailingMustBe(const char *expected) {
    EXPECT_STREQ(expected, _trailing);
  }