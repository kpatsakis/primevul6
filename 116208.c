TEST(FormatterTest, FormatHex) {
  EXPECT_EQ("0", format("{0:x}", 0));
  EXPECT_EQ("42", format("{0:x}", 0x42));
  EXPECT_EQ("42", format("{0:x}", 0x42u));
  EXPECT_EQ("-42", format("{0:x}", -0x42));
  EXPECT_EQ("12345678", format("{0:x}", 0x12345678));
  EXPECT_EQ("90abcdef", format("{0:x}", 0x90abcdef));
  EXPECT_EQ("12345678", format("{0:X}", 0x12345678));
  EXPECT_EQ("90ABCDEF", format("{0:X}", 0x90ABCDEF));

  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "-%x", 0 - static_cast<unsigned>(INT_MIN));
  EXPECT_EQ(buffer, format("{0:x}", INT_MIN));
  safe_sprintf(buffer, "%x", INT_MAX);
  EXPECT_EQ(buffer, format("{0:x}", INT_MAX));
  safe_sprintf(buffer, "%x", UINT_MAX);
  EXPECT_EQ(buffer, format("{0:x}", UINT_MAX));
  safe_sprintf(buffer, "-%lx", 0 - static_cast<unsigned long>(LONG_MIN));
  EXPECT_EQ(buffer, format("{0:x}", LONG_MIN));
  safe_sprintf(buffer, "%lx", LONG_MAX);
  EXPECT_EQ(buffer, format("{0:x}", LONG_MAX));
  safe_sprintf(buffer, "%lx", ULONG_MAX);
  EXPECT_EQ(buffer, format("{0:x}", ULONG_MAX));
}