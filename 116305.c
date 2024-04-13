TEST(FormatterTest, HashFlag) {
  EXPECT_EQ("42", format("{0:#}", 42));
  EXPECT_EQ("-42", format("{0:#}", -42));
  EXPECT_EQ("0b101010", format("{0:#b}", 42));
  EXPECT_EQ("0B101010", format("{0:#B}", 42));
  EXPECT_EQ("-0b101010", format("{0:#b}", -42));
  EXPECT_EQ("0x42", format("{0:#x}", 0x42));
  EXPECT_EQ("0X42", format("{0:#X}", 0x42));
  EXPECT_EQ("-0x42", format("{0:#x}", -0x42));
  EXPECT_EQ("042", format("{0:#o}", 042));
  EXPECT_EQ("-042", format("{0:#o}", -042));
  EXPECT_EQ("42", format("{0:#}", 42u));
  EXPECT_EQ("0x42", format("{0:#x}", 0x42u));
  EXPECT_EQ("042", format("{0:#o}", 042u));

  EXPECT_EQ("-42", format("{0:#}", -42l));
  EXPECT_EQ("0x42", format("{0:#x}", 0x42l));
  EXPECT_EQ("-0x42", format("{0:#x}", -0x42l));
  EXPECT_EQ("042", format("{0:#o}", 042l));
  EXPECT_EQ("-042", format("{0:#o}", -042l));
  EXPECT_EQ("42", format("{0:#}", 42ul));
  EXPECT_EQ("0x42", format("{0:#x}", 0x42ul));
  EXPECT_EQ("042", format("{0:#o}", 042ul));

  EXPECT_EQ("-42", format("{0:#}", -42ll));
  EXPECT_EQ("0x42", format("{0:#x}", 0x42ll));
  EXPECT_EQ("-0x42", format("{0:#x}", -0x42ll));
  EXPECT_EQ("042", format("{0:#o}", 042ll));
  EXPECT_EQ("-042", format("{0:#o}", -042ll));
  EXPECT_EQ("42", format("{0:#}", 42ull));
  EXPECT_EQ("0x42", format("{0:#x}", 0x42ull));
  EXPECT_EQ("042", format("{0:#o}", 042ull));

  EXPECT_EQ("-42.0000", format("{0:#}", -42.0));
  EXPECT_EQ("-42.0000", format("{0:#}", -42.0l));
  EXPECT_THROW_MSG(format("{0:#", 'c'),
      FormatError, "missing '}' in format string");
  EXPECT_THROW_MSG(format("{0:#}", 'c'),
      FormatError, "invalid format specifier for char");
  EXPECT_THROW_MSG(format("{0:#}", "abc"),
      FormatError, "format specifier '#' requires numeric argument");
  EXPECT_THROW_MSG(format("{0:#}", reinterpret_cast<void*>(0x42)),
      FormatError, "format specifier '#' requires numeric argument");
}