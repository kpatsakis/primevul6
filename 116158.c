TEST(FormatterTest, ZeroFlag) {
  EXPECT_EQ("42", format("{0:0}", 42));
  EXPECT_EQ("-0042", format("{0:05}", -42));
  EXPECT_EQ("00042", format("{0:05}", 42u));
  EXPECT_EQ("-0042", format("{0:05}", -42l));
  EXPECT_EQ("00042", format("{0:05}", 42ul));
  EXPECT_EQ("-0042", format("{0:05}", -42ll));
  EXPECT_EQ("00042", format("{0:05}", 42ull));
  EXPECT_EQ("-0042", format("{0:05}", -42.0));
  EXPECT_EQ("-0042", format("{0:05}", -42.0l));
  EXPECT_THROW_MSG(format("{0:0", 'c'),
      FormatError, "missing '}' in format string");
  EXPECT_THROW_MSG(format("{0:05}", 'c'),
      FormatError, "invalid format specifier for char");
  EXPECT_THROW_MSG(format("{0:05}", "abc"),
      FormatError, "format specifier '0' requires numeric argument");
  EXPECT_THROW_MSG(format("{0:05}", reinterpret_cast<void*>(0x42)),
      FormatError, "format specifier '0' requires numeric argument");
}