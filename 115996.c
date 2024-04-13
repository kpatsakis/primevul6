TEST(FormatterTest, PlusSign) {
  EXPECT_EQ("+42", format("{0:+}", 42));
  EXPECT_EQ("-42", format("{0:+}", -42));
  EXPECT_EQ("+42", format("{0:+}", 42));
  EXPECT_THROW_MSG(format("{0:+}", 42u),
      FormatError, "format specifier '+' requires signed argument");
  EXPECT_EQ("+42", format("{0:+}", 42l));
  EXPECT_THROW_MSG(format("{0:+}", 42ul),
      FormatError, "format specifier '+' requires signed argument");
  EXPECT_EQ("+42", format("{0:+}", 42ll));
  EXPECT_THROW_MSG(format("{0:+}", 42ull),
      FormatError, "format specifier '+' requires signed argument");
  EXPECT_EQ("+42", format("{0:+}", 42.0));
  EXPECT_EQ("+42", format("{0:+}", 42.0l));
  EXPECT_THROW_MSG(format("{0:+", 'c'),
      FormatError, "missing '}' in format string");
  EXPECT_THROW_MSG(format("{0:+}", 'c'),
      FormatError, "invalid format specifier for char");
  EXPECT_THROW_MSG(format("{0:+}", "abc"),
      FormatError, "format specifier '+' requires numeric argument");
  EXPECT_THROW_MSG(format("{0:+}", reinterpret_cast<void*>(0x42)),
      FormatError, "format specifier '+' requires numeric argument");
}