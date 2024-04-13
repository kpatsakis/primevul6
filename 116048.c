TEST(FormatterTest, FormatDec) {
  EXPECT_EQ("0", format("{0}", 0));
  EXPECT_EQ("42", format("{0}", 42));
  EXPECT_EQ("42", format("{0:d}", 42));
  EXPECT_EQ("42", format("{0}", 42u));
  EXPECT_EQ("-42", format("{0}", -42));
  EXPECT_EQ("12345", format("{0}", 12345));
  EXPECT_EQ("67890", format("{0}", 67890));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%d", INT_MIN);
  EXPECT_EQ(buffer, format("{0}", INT_MIN));
  safe_sprintf(buffer, "%d", INT_MAX);
  EXPECT_EQ(buffer, format("{0}", INT_MAX));
  safe_sprintf(buffer, "%u", UINT_MAX);
  EXPECT_EQ(buffer, format("{0}", UINT_MAX));
  safe_sprintf(buffer, "%ld", 0 - static_cast<unsigned long>(LONG_MIN));
  EXPECT_EQ(buffer, format("{0}", LONG_MIN));
  safe_sprintf(buffer, "%ld", LONG_MAX);
  EXPECT_EQ(buffer, format("{0}", LONG_MAX));
  safe_sprintf(buffer, "%lu", ULONG_MAX);
  EXPECT_EQ(buffer, format("{0}", ULONG_MAX));
}