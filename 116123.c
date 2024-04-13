TEST(FormatterTest, FormatOct) {
  EXPECT_EQ("0", format("{0:o}", 0));
  EXPECT_EQ("42", format("{0:o}", 042));
  EXPECT_EQ("42", format("{0:o}", 042u));
  EXPECT_EQ("-42", format("{0:o}", -042));
  EXPECT_EQ("12345670", format("{0:o}", 012345670));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "-%o", 0 - static_cast<unsigned>(INT_MIN));
  EXPECT_EQ(buffer, format("{0:o}", INT_MIN));
  safe_sprintf(buffer, "%o", INT_MAX);
  EXPECT_EQ(buffer, format("{0:o}", INT_MAX));
  safe_sprintf(buffer, "%o", UINT_MAX);
  EXPECT_EQ(buffer, format("{0:o}", UINT_MAX));
  safe_sprintf(buffer, "-%lo", 0 - static_cast<unsigned long>(LONG_MIN));
  EXPECT_EQ(buffer, format("{0:o}", LONG_MIN));
  safe_sprintf(buffer, "%lo", LONG_MAX);
  EXPECT_EQ(buffer, format("{0:o}", LONG_MAX));
  safe_sprintf(buffer, "%lo", ULONG_MAX);
  EXPECT_EQ(buffer, format("{0:o}", ULONG_MAX));
}